/*
 * Based on Code Manipulation API Sample (syscall.c) from DynamoRIO
 */

#include <assert.h>
#include <regex.h>
#include <errno.h>
#include <stdatomic.h>
#include <string.h> /* memset */
#include <immintrin.h> // For _mm_pause

#include "dr_api.h"
#include "drmgr.h"
#include "shm_string-macro.h"

#include "buffer.h"
#include "client.h"
#include "signatures.h"
#include "source.h"
#include "utils.h"
#include "list-embedded.h"
#include "vector-macro.h"
#include "spsc_ringbuf.h"
#include "streams/stream-drregex.h" /* event type */

#define warn(...) dr_fprintf(STDERR, "warning: " __VA_ARGS__)
#define info(...) dr_fprintf(STDERR, __VA_ARGS__)

#ifdef UNIX
#if defined(MACOS) || defined(ANDROID)
#include <sys/syscall.h>
#else
#include <syscall.h>
#endif
#endif

/* Some syscalls have more args, but this is the max we need for
 * SYS_write/NtWriteFile */
#ifdef WINDOWS
#define SYS_MAX_ARGS 9
#else
#define SYS_MAX_ARGS 3
#endif

#ifndef WITH_STDOUT
//#define dr_printf(...) do{}while(0)
#endif

#define MAXMATCH 20

static void *xmalloc(size_t sz) {
    assert(sz > 0);
    void *mem = malloc(sz);
    if (!mem) {
        assert(0 && "Allocation failed");
        abort();
    }
    return mem;
}


static char  *tmpline;
static size_t tmpline_len;

struct line {
    STRING(data);

    shm_list_embedded list;
};

struct line lines[3];
VEC(line_pool, struct line *);

static struct line *current_line[3];

#ifndef NDEBUG
static size_t pool_max_size;
#endif

bool first_match_only = true;
bool timestamps       = false;

const char *shmkey;

/* Thread-context-local storage index from drmgr */
static int tcls_idx;
/* we'll number threads from 0 up */
static size_t thread_num = 0;

static _Atomic bool  _list_lock = false;
static _Atomic bool  _pool_lock = false;

static inline void _lock(_Atomic bool *l) {
    while (atomic_exchange_explicit(l, true, memory_order_acquire))
        ;
}

static inline void _unlock(_Atomic bool *l) {
    atomic_store_explicit(l, false, memory_order_release);
}

static inline void list_lock() {
    _lock(&_list_lock);
}

static inline void list_unlock() {
    _unlock(&_list_lock);
}

static inline void pool_lock() {
    _lock(&_pool_lock);
}

static inline void pool_unlock() {
    _unlock(&_pool_lock);
}

//static _Atomic bool  _write_lock = false;

static inline void write_lock() {
    /* We have just one thread */
    //_lock(&_write_lock);
}

static inline void write_unlock() {
    //_unlock(&_write_lock);
}

/* The system call number of SYS_write/NtWriteFile */
static int write_sysnum, read_sysnum;

/* for some reason we need this...*/
#undef bool
#define bool char

static int  get_write_sysnum(void);
static int  get_read_sysnum(void);
static void event_exit(void);
static bool event_filter_syscall(void *drcontext, int sysnum);
static bool event_pre_syscall(void *drcontext, int sysnum);
static void event_post_syscall(void *drcontext, int sysnum);
static void event_thread_context_init(void *drcontext, bool new_depth);
static void event_thread_context_exit(void *drcontext, bool process_exit);

static void usage_and_exit(int ret) {
    dr_fprintf(STDERR,
               "Usage: drrun [-t] shmkey name expr sig [name expr sig] ...\n");
    exit(ret);
}

char **exprs[3];
char **names[3];
static size_t        exprs_num[3];
static regex_t      *re[3];
static char        **signatures[3];
struct event_record *events[3];
static size_t        waiting_for_buffer[3];
static shm_event     evs[3];

static struct buffer *shmbuf[3];

typedef struct {
    int    fd;
    void  *buf;
    size_t size;
    ssize_t len;
    size_t thread;
} per_thread_t;

#ifdef SUPPORT_MT
static _Atomic uint64_t timestamp = 1;
#else
static uint64_t timestamp = 1;
#endif

static int parse_line(int fd, char *line) {
    assert(fd >= 0 && fd < 3);

    int               status;
    signature_operand op;
    ssize_t           len;
    regmatch_t        matches[MAXMATCH + 1];

    int            num = (int)exprs_num[fd];
    struct buffer *shm = shmbuf[fd];

    // info("[%d] parsing line (%p): '%s'\n", fd, line, line);

    shm_event *ev = &evs[fd];
    for (int i = 0; i < num; ++i) {
        if ((ev->kind = events[fd][i].kind) == 0) {
            continue; /* monitor is not interested in this */
        }

        status = regexec(&re[fd][i], line, MAXMATCH, matches, 0);
        if (status != 0) {
            continue;
        }

#ifdef SUPPORT_MT
            uint64_t ts = atomic_fetch_and_add(&timestamp, 1, memory_order_seq_cst);
#else
            uint64_t ts = ++timestamp;
#endif
        int   m = 1;
        void *addr;

        size_t      waiting = 0;
        const char *o       = signatures[fd][i];
        /** LOCKED --
         * FIXME: we hold the lock long, first create the event locally and only
         * then push it **/
        write_lock();

        while (!(addr = buffer_start_push(shm))) {
            ++waiting_for_buffer[fd];
            if (++waiting > 5000) {
                if (!buffer_monitor_attached(shm)) {
                    warn("buffer detached while waiting for space");
                    write_unlock();
                    return -1;
                }
                waiting = 0;
            }
        }
        /* push the base info about event */
        ++ev->id;
        addr = buffer_partial_push(shm, addr, ev, sizeof(*ev));
        if (timestamps) {
            assert(*o == 't');
            addr = buffer_partial_push(shm, addr, &ts, sizeof(ts));
            ++o;
        }

        /* push the arguments of the event */
        for (; *o && m <= MAXMATCH; ++o, ++m) {
            if (*o == 'L') { /* user wants the whole line */
                addr = buffer_partial_push_str(shm, addr, ev->id, line);
                continue;
            }
            if (*o != 'M') {
                if ((int)matches[m].rm_so < 0) {
                    warn("have no match for '%c' in signature %s\n", *o,
                         signatures[fd][i]);
                    continue;
                }
                len = matches[m].rm_eo - matches[m].rm_so;
            } else {
                len = matches[0].rm_eo - matches[0].rm_so;
            }

            /* make sure we have big enough temporary buffer */
            if (tmpline_len < (size_t)len) {
                free(tmpline);
                tmpline = xmalloc(sizeof(char) * len + 1);
                tmpline_len = len;
            }

            if (*o == 'M') { /* user wants the whole match */
                assert(matches[0].rm_so >= 0);
                strncpy(tmpline, line + matches[0].rm_so, len);
                tmpline[len] = '\0';
                addr = buffer_partial_push_str(shm, addr, ev->id, tmpline);
                continue;
            } else {
                strncpy(tmpline, line + matches[m].rm_so, len);
                tmpline[len] = '\0';
            }

            switch (*o) {
            case 'c':
                assert(len == 1);
                addr = buffer_partial_push(
                    shm, addr, (char *)(line + matches[m].rm_eo), sizeof(op.c));
                break;
            case 'i':
                op.i = atoi(tmpline);
                addr = buffer_partial_push(shm, addr, &op.i, sizeof(op.i));
                break;
            case 'l':
                op.l = atol(tmpline);
                addr = buffer_partial_push(shm, addr, &op.l, sizeof(op.l));
                break;
            case 'f':
                op.f = atof(tmpline);
                addr = buffer_partial_push(shm, addr, &op.f, sizeof(op.f));
                break;
            case 'd':
                op.d = strtod(tmpline, NULL);
                addr = buffer_partial_push(shm, addr, &op.d, sizeof(op.d));
                break;
            case 'S':
                addr = buffer_partial_push_str(shm, addr, ev->id, tmpline);
                break;
            default:
                assert(0 && "Invalid signature");
            }
        }
        buffer_finish_push(shm);
        write_unlock();

        if (first_match_only)
            break;
    }

    return 0;
}

/*
static void dump_line(struct line *line) {
    printf("[%p, len %lu] ", line, STRING_SIZE(line->data));
    printf("'%.*s'\n",
           STRING_SIZE(line->data) > 10 ? 10 : (int)STRING_SIZE(line->data),
           line->data);
}

static void dump_lines(int fd) {
    int n = 0;
    struct line *line;
    info("Lines [%d]:\n", fd);
    shm_list_embedded_foreach(line, &lines[fd].list, list) {
        printf("  fd %d, line %d: ", fd, ++n);
        dump_line(line);
    }
    info("----\n");
}
*/

static inline void put_to_pool(struct line *line) {
    /* clear the string */
    STRING_SIZE(line->data) = 0;

    pool_lock();
    VEC_PUSH(line_pool, &line);
#ifndef NDEBUG
    if (VEC_SIZE(line_pool) > pool_max_size) {
        pool_max_size = VEC_SIZE(line_pool);
    }
#endif
    pool_unlock();
}


static inline struct line *get_line(int fd) {
    struct line *line;
    shm_list_embedded_foreach(line, &lines[fd].list, list) {
        return line;
    }
    return NULL;
}

static bool monitor_disconected() {
    for (int i = 0; i < 3; ++i) {
        if (shmbuf[i] == 0)
            continue;
        if (buffer_monitor_attached(shmbuf[i])) {
            return false;
        }
    }
    return true;
}

static volatile _Atomic int __parser_finished = 0;

_Atomic bool __done[3];

static inline bool all_done() {
    for (int i = 0; i < 3; ++i) {
        if (atomic_load_explicit(&__done[i], memory_order_acquire) == 0) {
            return false;
        }
    }
    return true;
}

static void parser_thread(void *data) {
    (void)data;
    size_t no_line = 0;
    struct line *line;

    for (int i = 0; i < 3; ++i) {
        __done[i] = (shmbuf[i] == 0);
    }

    while (1) {
        for (int i = 0; i < 3; ++i) {
            if (shmbuf[i] == 0) {
                continue;
            }

            while (1) {
                list_lock();
                line = get_line(i);
                if (!line) {
                    list_unlock();
                    break;
                }

                assert(!shm_list_embedded_empty(&lines[i].list));
                shm_list_embedded_remove(&line->list);
                list_unlock();

                assert(line);
                //info("parse line: '%s'\n", line->data);
                if (parse_line(i, line->data) < 0) {
                    warn("parse line returned error\n");
                    goto finish;
                }
                /* TODO: insert into pool */
                put_to_pool(line);
                no_line = 0;
            }
        }

        if (++no_line > 10) {
            if (no_line > 1000) {
                if (all_done())
                    goto finish;
                if (monitor_disconected()) {
                    warn("Parser thread: all disconnected, exitting...\n");
                    goto finish;
                }

                dr_sleep(5);
                if (no_line > 1000000) {
                    dr_sleep(10);
                    info("no line long time\n");
                }
            }

        }
    }

finish:
    __parser_finished = 1;
}



struct line *create_new_line() {
    struct line *line = xmalloc(sizeof *line);
    STRING_INIT(line->data);
    STRING_GROW(line->data, 128);
    shm_list_embedded_init(&line->list);

    return line;
}

static struct line *init_new_line(int fd) {
    struct line *line = NULL;
    pool_lock();
    if (VEC_SIZE(line_pool) > 0) {
        line = VEC_POP_TOP(line_pool);
    }
    pool_unlock();

    if (line == NULL) {
        line = create_new_line();
    }

    current_line[fd] = line;
    return line;
}

static inline void finish_line(int fd) {
    list_lock();
    /* insert at the end */
    shm_list_embedded_insert_after(lines[fd].list.prev, &current_line[fd]->list);
    list_unlock();
}

static void handle_event(per_thread_t *data) {
    DR_ASSERT(data->len > 0);
    /*
    info("---- [fd: %d, len: %ld, size: %lu\n]"
                 "'%*s'\n",
            data->fd, data->len, data->size, (int)data->len, (char*)data->buf);
            */

    const int fd = data->fd;
    assert(fd >= 0 && fd < 3);

    size_t n = 0;
    const size_t data_len = data->len;
    struct line *line = current_line[fd];
    size_t space = 0;
    while (n < data_len) {
        if (space == 0) {
            STRING_ENSURE_SPACE(line->data);
            assert(STRING_SIZE(line->data) < STRING_ALLOC_SIZE(line->data));
            space = STRING_ALLOC_SIZE(line->data) - STRING_SIZE(line->data);
            assert(space > 0);
        }

        while (n < data_len && space > 0) {
            char c = ((char*)data->buf)[n++];
            --space;

            if (c == '\n' || c == '\0') {
                assert(STRING_SIZE(line->data) < STRING_ALLOC_SIZE(line->data));
                ++STRING_SIZE(line->data);
                STRING_TOP(line->data) = '\0';

                /* finish this line and start a new one */
                finish_line(fd);
                line = init_new_line(fd);
                assert(STRING_SIZE(line->data) == 0);

                /* break this loop and let the outer loop
                 * recompute available space */
                space = 0;
                break;
            }

            assert(STRING_SIZE(line->data) < STRING_ALLOC_SIZE(line->data));
            ++STRING_SIZE(line->data);
            STRING_TOP(line->data) = c;
        }
    }
}

int parse_args(int argc, const char *argv[], char **exprs[3], char **names[3]) {

    int      arg_i, cur_fd = 1;
    int      args_i[3] = {0, 0, 0};
    int i         = 1;
    /*should the events be enriched with timestamps? */
    if (strncmp(argv[i], "-t", 3) == 0) {
        ++i;
        timestamps = true;
    }
    shmkey = argv[i++];

    for (; i < argc; ++i) {
        if (strncmp(argv[i], "-stdin", 7) == 0) {
            cur_fd = 0;
            continue;
        }
        if (strncmp(argv[i], "-stdout", 7) == 0) {
            cur_fd = 1;
            continue;
        }
        if (strncmp(argv[i], "-stderr", 7) == 0) {
            cur_fd = 2;
            continue;
        }

        /* this is the begining of event def */
        arg_i                = args_i[cur_fd];
        names[cur_fd][arg_i] = (char*)argv[i++];
        exprs[cur_fd][arg_i] = (char*)argv[i++];

        /* +2 for 0 byte and possibly "t" for timestamp */
        signatures[cur_fd][arg_i] = xmalloc(sizeof(char) * strlen(argv[i]) + 2);
        sprintf(signatures[cur_fd][arg_i], timestamps ? "t%s" : "%s", argv[i]);

        /* compile the regex, use extended RE */
        int status =
            regcomp(&re[cur_fd][arg_i], exprs[cur_fd][arg_i], REG_EXTENDED);
        if (status != 0) {
            warn("failed compiling regex '%s'\n", exprs[cur_fd][arg_i]);
            for (int tmp = 0; tmp < arg_i; ++tmp) {
                regfree(&re[cur_fd][tmp]);
            }
            return -1;
        }

        ++args_i[cur_fd];
    }

    assert(exprs_num[0] == (size_t)args_i[0]);
    assert(exprs_num[1] == (size_t)args_i[1]);
    assert(exprs_num[2] == (size_t)args_i[2]);

    return 0;
}

static const char *fd_to_name(int i) {
    return (i == 0 ? "stdin" : (i == 1 ? "stdout" : "stderr"));
}

int get_exprs_num(int argc, const char *argv[]) {
    int      n      = 0;
    int      cur_fd = 1;
    int i      = 1;
    if (strncmp(argv[i], "-t", 3) == 0) {
        ++i;
    }

    ++i; /* skip shmkey */

    for (; i < argc; ++i) {
        if (strncmp(argv[i], "--", 3) == 0)
            break;
        if (strncmp(argv[i], "-stdin", 7) == 0) {
            cur_fd = 0;
            continue;
        }
        if (strncmp(argv[i], "-stdout", 7) == 0) {
            cur_fd = 1;
            continue;
        }
        if (strncmp(argv[i], "-stderr", 7) == 0) {
            cur_fd = 2;
            continue;
        }

        /* this must be the event name */
        ++n;
        ++exprs_num[cur_fd];
        i += 2; /* skip regex and signature */
        if (i >= argc) {
            warn("invalid number of arguments\n");
            return -1;
        }
    }

    return n;
}

DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[]) {
    (void)id;
    dr_set_client_name("Shamon intercept write and read syscalls",
                       "http://...");
    drmgr_init();
    write_sysnum = get_write_sysnum();
    read_sysnum  = get_read_sysnum();
    dr_register_filter_syscall_event(event_filter_syscall);
    drmgr_register_pre_syscall_event(event_pre_syscall);
    drmgr_register_post_syscall_event(event_post_syscall);
    dr_register_exit_event(event_exit);
    tcls_idx = drmgr_register_cls_field(event_thread_context_init,
                                        event_thread_context_exit);
    DR_ASSERT(tcls_idx != -1);
    if (dr_is_notify_on()) {
#ifdef WINDOWS
        /* ask for best-effort printing to cmd window.  must be called at init.
         */
        dr_enable_console_printing();
#endif
        info("Client Shamon-drrregex is running\n");
    }

    if (argc < 6) {
        usage_and_exit(1);
    }

    if (get_exprs_num(argc, argv) <= 0) {
        usage_and_exit(1);
    }

    char **exprs[3];
    char **names[3];
    size_t num;
    for (int i = 0; i < 3; ++i) {
        num = exprs_num[i];
        if (num == 0)
            continue;

        exprs[i] = xmalloc(num * sizeof(char *));
        names[i] = xmalloc(num * sizeof(char *));
        signatures[i] = xmalloc(num * sizeof(char *));
        re[i] = xmalloc(num * sizeof(regex_t));

        STRING_INIT(lines[i].data);
        STRING_GROW(lines[i].data, 128);
        shm_list_embedded_init(&lines[i].list);
        init_new_line(i);
    }

    int err = parse_args(argc, argv, exprs, names);
    if (err < 0) {
        usage_and_exit(1);
    }

    if (!shmkey || shmkey[0] != '/') {
        usage_and_exit(1);
    }

    char extended_shmkey[256];
    int  filter_fd_mask = 0;

    info("Creating SHM buffers\n");

    /* Create shared memory buffers */
    for (int i = 0; i < 3; ++i) {
        if (exprs_num[i] == 0) {
            /* we DONT want to get data from this fd from eBPF */
            filter_fd_mask |= (1 << i);
            continue;
        }

        /* Initialize the info about this source */
        struct source_control *control = source_control_define_pairwise(
            exprs_num[i], (const char **)names[i],
            (const char **)signatures[i]);
        assert(control);

        int ret =
            snprintf(extended_shmkey, 256, "%s.%s", shmkey, fd_to_name(i));
        if (ret < 0 || ret >= 256) {
            warn("ERROR: SHM key is too long\n");
            return;
        }

        shmbuf[i] = create_shared_buffer(extended_shmkey, control);
        /* create the shared buffer */
        assert(shmbuf[i]);

        size_t events_num;
        events[i] = buffer_get_avail_events(shmbuf[i], &events_num);
        free(control);
    }

    info("waiting for the monitor to attach... ");
    for (int i = 0; i < 3; ++i) {
        if (shmbuf[i] == NULL)
            continue;
        err = buffer_wait_for_monitor(shmbuf[i]);
        if (err < 0) {
            if (err != EINTR) {
                warn("failed waiting: %s\n", strerror(-err));
                /* simulate the end of thread so that event_exit()
                 * does not wait for that */
                __parser_finished = 1;
                event_exit();
                exit(1);
            }
            return;
        }
    }
    info("done\n");

    info("Creating parser thread...");
    if (!dr_create_client_thread(parser_thread, 0)) {
        warn("failed creating the parser thread\n");
        abort();
    }
    info("done\n");
    info("Continue program\n");
}

static void event_exit(void) {
    /* notify thread that this is the end */
    for (int i = 0; i < 3; ++i) {
        if (shmbuf[i] == 0)
            continue;
        atomic_store_explicit(&__done[i], 1, memory_order_release);
    }

    info("Waiting for thread...");
    /* wait until the thread finishes */
    while (!__parser_finished) {
        dr_sleep(5);
    }
    info(" finished!\n");

#ifndef NDEBUG
    for (int i = 0; i < 3; ++i) {
        if (shmbuf[i] == 0)
            continue;
        assert(shm_list_embedded_empty(&lines[i].list) &&
               "Have unprocessed lines");
    }
#endif

    if (!drmgr_unregister_cls_field(event_thread_context_init,
                                    event_thread_context_exit, tcls_idx) ||
        !drmgr_unregister_pre_syscall_event(event_pre_syscall) ||
        !drmgr_unregister_post_syscall_event(event_post_syscall))
        DR_ASSERT(false && "failed to unregister");
    drmgr_exit();

    for (unsigned fd = 0; fd < 3; ++fd) {
        if (shmbuf[fd] == NULL)
            continue;

        info(
            "[fd %d] info: sent %lu events, busy waited on buffer %lu cycles\n",
            fd, evs[fd].id, waiting_for_buffer[fd]);

        destroy_shared_buffer(shmbuf[fd]);

        for (int i = 0; i < (int)exprs_num[fd]; ++i) {
            regfree(&re[fd][i]);
        }
        free(exprs[fd]);
        free(names[fd]);
        for (size_t j = 0; j < exprs_num[fd]; ++j) {
            free(signatures[fd][j]);
        }
        free(signatures[fd]);
        free(re[fd]);
    }

    VEC_DESTROY(line_pool);

    free(tmpline);
#ifndef NDEBUG
    info("Max pool size: %lu\n", pool_max_size);
#endif
    /*info("Clean up done\n");*/
}

static void event_thread_context_init(void *drcontext, bool new_depth) {
    /* create an instance of our data structure for this thread context */
    per_thread_t *data;
    if (new_depth) {
        data = (per_thread_t *)dr_thread_alloc(drcontext, sizeof(per_thread_t));
        drmgr_set_cls_field(drcontext, tcls_idx, data);
        data->fd     = -1;
        data->thread = thread_num++;
    } else {
        data = (per_thread_t *)drmgr_get_cls_field(drcontext, tcls_idx);
    }
}

static void event_thread_context_exit(void *drcontext, bool thread_exit) {
    if (!thread_exit)
        return;
    per_thread_t *data =
        (per_thread_t *)drmgr_get_cls_field(drcontext, tcls_idx);
    dr_thread_free(drcontext, data, sizeof(per_thread_t));
}

static bool event_filter_syscall(void *drcontext, int sysnum) {
    (void)drcontext;
    return sysnum == write_sysnum || sysnum == read_sysnum;
}

static bool event_pre_syscall(void *drcontext, int sysnum) {
    /* do we need this check?  we have the filter... */
    if (sysnum != read_sysnum && sysnum != write_sysnum) {
        return true;
    }

    reg_t         fd   = dr_syscall_get_param(drcontext, 0);
    reg_t         buf  = dr_syscall_get_param(drcontext, 1);
    reg_t         size = dr_syscall_get_param(drcontext, 2);
    per_thread_t *data =
        (per_thread_t *)drmgr_get_cls_field(drcontext, tcls_idx);
    data->fd   = fd; /* store the fd for post-event */
    data->buf  = (void *)buf;
    data->size = size;
    return true; /* execute normally */
}

static void event_post_syscall(void *drcontext, int sysnum) {
    reg_t retval = dr_syscall_get_result(drcontext);

    if (sysnum != read_sysnum && sysnum != write_sysnum) {
        return;
    }
    per_thread_t *data =
        (per_thread_t *)drmgr_get_cls_field(drcontext, tcls_idx);

    if (data->fd > 2)
        return;

    data->len = *((ssize_t *)&retval);
    if (data->len <= 0)
        return;

    // dr_printf("Syscall: %i; len: %li; result: %lu\n",sysnum, len, len);
    handle_event(data);
}

static int get_write_sysnum(void) {
    /* XXX: we could use the "drsyscall" Extension from the Dr. Memory Framework
     * (DRMF) to obtain the number of any system call from the name.
     */
#ifdef UNIX
    return SYS_write;
#else
    byte          *entry;
    module_data_t *data = dr_lookup_module_by_name("ntdll.dll");
    DR_ASSERT(data != NULL);
    entry = (byte *)dr_get_proc_address(data->handle, "NtWriteFile");
    DR_ASSERT(entry != NULL);
    dr_free_module_data(data);
    return drmgr_decode_sysnum_from_wrapper(entry);
#endif
}

static int get_read_sysnum(void) {
#ifdef UNIX
    return SYS_read;
#else
    byte          *entry;
    module_data_t *data = dr_lookup_module_by_name("ntdll.dll");
    DR_ASSERT(data != NULL);
    entry = (byte *)dr_get_proc_address(data->handle, "NtReadFile");
    DR_ASSERT(entry != NULL);
    dr_free_module_data(data);
    return drmgr_decode_sysnum_from_wrapper(entry);
#endif
}
