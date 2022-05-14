#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <assert.h>

#include "shmbuf/buffer.h"
#include "shmbuf/client.h"
#include "event.h"
#include "signatures.h"
#include "source.h"

#define MAXMATCH 20

static void usage_and_exit(int ret) {
    fprintf(stderr, "Usage: regex shmkey name expr sig [name expr sig] ...\n");
    exit(ret);
}

// #define WITH_LINES
// #define WITH_STDOUT

#ifndef WITH_STDOUT
#define printf(...) do{}while(0)
#endif

struct event {
    shm_event base;
#ifdef WITH_LINES
    size_t line;
#endif
    unsigned char args[];
};


static size_t compute_elem_size(char *signatures[],
                                size_t num) {
    size_t size, max_size = 0;
    for (size_t i = 0; i < num; ++i) {
        size = signature_get_size((const unsigned char*)signatures[i]) +
                                  sizeof(struct event);
        if (size > max_size)
            max_size = size;
    }

    if (max_size < sizeof(shm_event_dropped))
        max_size = sizeof(shm_event_dropped);

    return max_size;
}

static size_t waiting_for_buffer = 0;

int main (int argc, char *argv[]) {
    if (argc < 5 && (argc - 2) % 3 != 0) {
        usage_and_exit(1);
    }

    size_t exprs_num = (argc-1)/3;
    if (exprs_num == 0) {
        usage_and_exit(1);
    }

    const char *shmkey = argv[1];
    char *exprs[exprs_num];
    char *signatures[exprs_num];
    char *names[exprs_num];
    regex_t re[exprs_num];

    int arg_i = 2;
    for (int i = 0; i < (int)exprs_num; ++i) {
        names[i] = argv[arg_i++];
        exprs[i] = argv[arg_i++];
        if (arg_i >= argc) {
            fprintf(stderr, "Missing a signature for '%s'\n", exprs[i]);
            usage_and_exit(1);
        }
        signatures[i] = argv[arg_i++];

        /* compile the regex, use extended RE */
        int status = regcomp(&re[i], exprs[i], REG_EXTENDED);
        if (status != 0) {
            fprintf(stderr, "Failed compiling regex '%s'\n", exprs[i]);
            /* FIXME: we leak the expressions compiled so far ... */
            exit(1);
        }
    }

    /* Initialize the info about this source */
    /* FIXME: do this more user-friendly */
    size_t control_size = sizeof(size_t) + sizeof(struct event_record)*exprs_num;
    struct source_control *control = initialize_shared_control_buffer(shmkey, control_size);
    assert(control);
    control->size = control_size;
    for (int i = 0; i < (int)exprs_num; ++i) {
        strncpy(control->events[i].name, names[i],
                sizeof(control->events[i].name));
#ifdef WITH_LINES
        assert(strlen(signatures[i]) + 1 <= sizeof(control->events[i].signature));
        control->events[i].signature[0] = 'l'; /* first param is line */

        strncpy((char*)control->events[i].signature + 1,
                signatures[i],
                sizeof(control->events[i].signature));
#else
        assert(strlen(signatures[i])  <= sizeof(control->events[i].signature));
        strncpy((char*)control->events[i].signature,
                signatures[i],
                sizeof(control->events[i].signature));
#endif
        control->events[i].kind = 0;
        control->events[i].size = signature_get_size((unsigned char*)signatures[i]) + sizeof(struct event);
    }

    (void)signatures;
    struct buffer *shm = initialize_shared_buffer(shmkey,
                                                  compute_elem_size(signatures, exprs_num),
                                                  control);
    assert(shm);
    fprintf(stderr, "info: waiting for the monitor to attach\n");
    buffer_wait_for_monitor(shm);

    regmatch_t matches[MAXMATCH+1];

    int status;
    ssize_t len;
    size_t line_len;
    char *line = NULL;
    char *tmpline = NULL;
    size_t tmpline_len = 0;
    signature_operand op;

    struct event ev;
    memset(&ev, 0, sizeof(ev));

    while (1) {
        len = getline(&line, &line_len, stdin);
        if (len == -1)
            break;
        if (len == 0)
            continue;

#ifdef WITH_LINES
        ++ev.line;
#endif

        /* remove newline from the line */
        line[len-1] = '\0';

        for (int i = 0; i < (int)exprs_num; ++i) {
            if (control->events[i].kind == 0)
                continue; /* monitor is not interested in this */

            status = regexec(&re[i], line, MAXMATCH, matches, 0);
            if (status != 0) {
                continue;
            }
            printf("{");
            int m = 1;
            void *addr;
            while (!(addr = buffer_start_push(shm))) {
                ++waiting_for_buffer;
            }
            /* push the base info about event */
            ++ev.base.id;
            ev.base.kind = control->events[i].kind;
            addr = buffer_partial_push(shm, addr, &ev, sizeof(ev));

            /* push the arguments of the event */
            for (const char *o = signatures[i]; *o && m <= MAXMATCH; ++o, ++m) {
                if (m > 1)
                    printf(", ");

                if (*o == 'L') { /* user wants the whole line */
                    printf("'%s'", line);
                    addr = buffer_partial_push_str(shm, addr, ev.base.id, line);
                    continue;
                }
                if (*o != 'M') {
                    if ((int)matches[m].rm_so < 0) {
                        fprintf(stderr, "warning: have no match for '%c' in signature %s\n", *o, signatures[i]);
                        continue;
                    }
                    len = matches[m].rm_eo - matches[m].rm_so;
                } else {
                    len = matches[0].rm_eo - matches[0].rm_so;
                }

                /* make sure we have big enough temporary buffer */
                if (tmpline_len < (size_t)len) {
                    free(tmpline);
                    tmpline = malloc(sizeof(char)*len+1);
                    assert(tmpline && "Memory allocation failed");
                    tmpline_len = len;
                }

                if (*o == 'M') { /* user wants the whole match */
                    assert(matches[0].rm_so >= 0);
                    strncpy(tmpline, line+matches[0].rm_so, len);
                    tmpline[len] = '\0';
                    addr = buffer_partial_push_str(shm, addr, ev.base.id, tmpline);
                    printf("'%s'",  tmpline);
                    continue;
                } else {
                    strncpy(tmpline, line+matches[m].rm_so, len);
                    tmpline[len] = '\0';
                }

                switch(*o) {
                    case 'c':
                        assert(len == 1);
                        printf("%c", *(char*)(line + matches[m].rm_eo));
                        addr = buffer_partial_push(shm, addr,
                                                   (char*)(line + matches[m].rm_eo),
                                                   sizeof(op.c));
                        break;
                    case 'i': op.i = atoi(tmpline);
                              printf("%d", op.i);
                              addr = buffer_partial_push(shm, addr, &op.i, sizeof(op.i));
                              break;
                    case 'l': op.l = atol(tmpline);
                              printf("%ld", op.l);
                              addr = buffer_partial_push(shm, addr, &op.l, sizeof(op.l));
                              break;
                    case 'f': op.f = atof(tmpline);
                              printf("%lf", op.f);
                              addr = buffer_partial_push(shm, addr, &op.f, sizeof(op.f));
                              break;
                    case 'd': op.d = strtod(tmpline, NULL);
                              printf("%lf", op.d);
                              addr = buffer_partial_push(shm, addr, &op.d, sizeof(op.d));
                              break;
                    case 'S': printf("'%s'", tmpline);
                              addr = buffer_partial_push_str(shm, addr, ev.base.id, tmpline);
                              break;
                    default: assert(0 && "Invalid signature");
                }
            }
            buffer_finish_push(shm);
            printf("}\n");
        }
    }

    /* Free up memory held within the regex memory */
    fprintf(stderr, "info: sent %lu events, busy waited on buffer %lu cycles\n",
            ev.base.id, waiting_for_buffer);
    free(tmpline);
    free(line);
    for (int i = 0; i < (int)exprs_num; ++i) {
        regfree(&re[i]);
    }

    destroy_shared_buffer(shm);

    return 0;

}
