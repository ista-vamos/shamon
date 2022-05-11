#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <assert.h>

#include "shmbuf/buffer.h"

#define MAXMATCH 20

static void usage_and_exit(int ret) {
    fprintf(stderr, "Usage: regex expr sig expr sig ...\n");
    exit(ret);
}

int main (int argc, char *argv[]) {
    if (argc < 2 && (argc - 1) % 2 != 0) {
        usage_and_exit(1);
    }

    size_t exprs_num = (argc-1)/2;
    if (exprs_num == 0) {
        usage_and_exit(1);
    }

    char *exprs[exprs_num];
    char *signatures[exprs_num];
    regex_t re[exprs_num];

    int arg_i = 1;
    for (int i = 0; i < (int)exprs_num; ++i) {
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

    (void)signatures;

    regmatch_t matches[MAXMATCH+1];

    int status;
    ssize_t len;
    size_t line_len;
    char *line = NULL;
    char *tmpline = NULL;
    size_t tmpline_len = 0;

    while (1) {
        len = getline(&line, &line_len, stdin);
        if (len == -1)
            return 0;
        if (len == 0)
            continue;

        /* remove newline from the line */
        line[len-1] = '\0';

        for (int i = 0; i < (int)exprs_num; ++i) {
            status = regexec(&re[i], line, MAXMATCH, matches, 0);
            if (status != 0) {
                continue;
            }
            printf("{");
            int m = 1;
            for (const char *o = signatures[i]; *o && m <= MAXMATCH; ++o, ++m) {
                if (m > 1)
                    printf(", ");

                if (*o == 'L') { /* user wants the whole line */
                    printf("'%s'", line);
                    continue;
                }
                if (*o == 'M') { /* user wants the whole match */
                    assert(matches[0].rm_so >= 0);
                    len = matches[0].rm_eo - matches[0].rm_so;
                    printf("'%.*s'",  (int)len, line+matches[0].rm_so);
                    continue;
                }

                if ((int)matches[m].rm_so < 0) {
                    fprintf(stderr, "warning: have no match for '%c' in signature %s\n", *o, signatures[i]);
                    continue;
                }

                /* make sure we have big enough temporary buffer */
                len = matches[m].rm_eo - matches[m].rm_so;
                if (tmpline_len < (size_t)len) {
                    free(tmpline);
                    tmpline = malloc(sizeof(char)*len+1);
                    assert(tmpline && "Memory allocation failed");
                    tmpline_len = len;
                }

                strncpy(tmpline, line+matches[m].rm_so, len);
                tmpline[len] = '\0';
                switch(*o) {
                    case 'c':
                        assert(len == 1);
                        printf("%c", *(char*)(line + matches[m].rm_eo));
                        break;
                    case 'i': printf("%d", atoi(tmpline)); break;
                    case 'l': printf("%ld", atol(tmpline)); break;
                    case 'f': printf("%f", atof(tmpline)); break;
                    case 'd': printf("%lf", strtod(tmpline, NULL)); break;
                    case 'S': printf("'%s'", tmpline); break;
                    default: assert(0 && "Invalid signature");
                }
            }
            printf("}\n");
        }
    }

    /* Free up memory held within the regex memory */

    free(tmpline);
    free(line);
    for (int i = 0; i < (int)exprs_num; ++i) {
        regfree(&re[i]);
    }

    return 0;

}
