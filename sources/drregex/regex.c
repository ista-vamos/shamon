/* 
 * Based on Code Manipulation API Sample (syscall.c) from DynamoRIO
 */

#include <string.h> /* memset */
#include <regex.h>
#include <assert.h>

#include "dr_api.h"
#include "drmgr.h"

#include "event.h"
#include "buffer.h"
#include "client.h"
#include "signatures.h"
#include "source.h"

#include "../fastbuf/shm_monitored.h"

#ifdef UNIX
#    if defined(MACOS) || defined(ANDROID)
#        include <sys/syscall.h>
#    else
#        include <syscall.h>
#    endif
#endif

/* Some syscalls have more args, but this is the max we need for SYS_write/NtWriteFile */
#ifdef WINDOWS
#    define SYS_MAX_ARGS 9
#else
#    define SYS_MAX_ARGS 3
#endif

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

typedef struct {
	int fd;
    void *buf;
    size_t size;
    size_t thread;
} per_thread_t;

/* Thread-context-local storage index from drmgr */
static int tcls_idx;
static size_t thread_num = 0;

/* shmbuf assumes one writer and one reader, but here we may have multiple writers
 * (multiple threads), so we must make sure they are seuqntialized somehow
   (until we have the implementation for multiple-writers) */
//static _Atomic(bool) write_lock = false;
static struct buffer *shm;

/* The system call number of SYS_write/NtWriteFile */
static int write_sysnum, read_sysnum;

/* for some reason we need this...*/
#undef bool
#define bool char

static int
get_write_sysnum(void);
static int
get_read_sysnum(void);
static void
event_exit(void);
static bool
event_filter_syscall(void *drcontext, int sysnum);
static bool
event_pre_syscall(void *drcontext, int sysnum);
static void
event_post_syscall(void *drcontext, int sysnum);
static void
event_thread_context_init(void *drcontext, bool new_depth);
static void
event_thread_context_exit(void *drcontext, bool process_exit);

static void usage_and_exit(int ret) {
    fprintf(stderr, "Usage: regex shmkey name expr sig [name expr sig] ...\n");
    exit(ret);
}

DR_EXPORT void
dr_client_main(client_id_t id, int argc, const char *argv[])
{
    (void)id;
    dr_set_client_name("Shamon intercept write and read syscalls", "http://...");
    drmgr_init();
    write_sysnum = get_write_sysnum();
    read_sysnum = get_read_sysnum();
    dr_register_filter_syscall_event(event_filter_syscall);
    drmgr_register_pre_syscall_event(event_pre_syscall);
    drmgr_register_post_syscall_event(event_post_syscall);
    dr_register_exit_event(event_exit);
    tcls_idx =
        drmgr_register_cls_field(event_thread_context_init, event_thread_context_exit);
    DR_ASSERT(tcls_idx != -1);
    if (dr_is_notify_on()) {
#    ifdef WINDOWS
        /* ask for best-effort printing to cmd window.  must be called at init. */
        dr_enable_console_printing();
#    endif
        dr_fprintf(STDERR, "Client DrRegex is running\n");
    }
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
        names[i] = (char*)argv[arg_i++];
        exprs[i] = (char*)argv[arg_i++];
        if (arg_i >= argc) {
            dr_fprintf(STDERR, "Missing a signature for '%s'\n", exprs[i]);
            usage_and_exit(1);
        }
        signatures[i] = (char*)argv[arg_i++];

        /* compile the regex, use extended RE */
        int status = regcomp(&re[i], exprs[i], REG_EXTENDED);
        if (status != 0) {
            dr_fprintf(STDERR, "Failed compiling regex '%s'\n", exprs[i]);
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
    shm = initialize_shared_buffer(shmkey,
                                   compute_elem_size(signatures, exprs_num),
                                   control);
    assert(shm);
    fprintf(stderr, "info: waiting for the monitor to attach\n");
    buffer_wait_for_monitor(shm);
}


static void
event_exit(void)
{
    if (!drmgr_unregister_cls_field(event_thread_context_init,
			    	    event_thread_context_exit,
                                    tcls_idx) ||
        !drmgr_unregister_pre_syscall_event(event_pre_syscall) ||
        !drmgr_unregister_post_syscall_event(event_post_syscall))
        DR_ASSERT(false && "failed to unregister");
    drmgr_exit();
    destroy_shared_buffer(shm);
}

static void
event_thread_context_init(void *drcontext, bool new_depth)
{
    /* create an instance of our data structure for this thread context */
    per_thread_t *data;
    if (new_depth) {
        data = (per_thread_t *)dr_thread_alloc(drcontext, sizeof(per_thread_t));
        drmgr_set_cls_field(drcontext, tcls_idx, data);
        data->fd = -1;
        data->thread = thread_num++;
        // FIXME: typo in the name
        // intialize_thread_buffer(1, 2);
    } else {
        data = (per_thread_t *)drmgr_get_cls_field(drcontext, tcls_idx);
    }
}

static void
event_thread_context_exit(void *drcontext, bool thread_exit)
{
    if (!thread_exit)
	    return;
    per_thread_t *data = (per_thread_t *)drmgr_get_cls_field(drcontext, tcls_idx);
    dr_thread_free(drcontext, data, sizeof(per_thread_t));
    //close_thread_buffer();
}

static bool
event_filter_syscall(void *drcontext, int sysnum)
{
    (void)drcontext;
    return sysnum == write_sysnum || sysnum == read_sysnum ;
}

static bool
event_pre_syscall(void *drcontext, int sysnum)
{
    /* do we need this check?  we have the filter... */
    if(sysnum != read_sysnum && sysnum != write_sysnum)
    {
        return true;
    }

    reg_t fd = dr_syscall_get_param(drcontext, 0);
    reg_t buf = dr_syscall_get_param(drcontext, 1);
    reg_t size = dr_syscall_get_param(drcontext, 2);
    per_thread_t *data = (per_thread_t *)drmgr_get_cls_field(drcontext, tcls_idx);
    data->fd = fd; /* store the fd for post-event */
    data->buf=(void*)buf;
    data->size=size;
    return true; /* execute normally */
}

static void
event_post_syscall(void *drcontext, int sysnum)
{
    reg_t retval = dr_syscall_get_result(drcontext);

    if(sysnum != read_sysnum && sysnum != write_sysnum)
    {
        return;
    }
    per_thread_t *data = (per_thread_t *)drmgr_get_cls_field(drcontext, tcls_idx);
    if(data->fd>2)
    {
        return;
    }
    ssize_t len = *((ssize_t*)&retval);
    dr_printf("Syscall: %i; len: %li; result: %lu\n",sysnum, len, len);
    if(sysnum==read_sysnum)
    {
        //push_read(data->fd, data->buf, data->size, len);
    }
    else if(sysnum==write_sysnum)
    {
        //push_write(data->fd, data->buf, data->size, len);
    }
}

static int
get_write_sysnum(void)
{
    /* XXX: we could use the "drsyscall" Extension from the Dr. Memory Framework
     * (DRMF) to obtain the number of any system call from the name.
     */
#ifdef UNIX
    return SYS_write;
#else
    byte *entry;
    module_data_t *data = dr_lookup_module_by_name("ntdll.dll");
    DR_ASSERT(data != NULL);
    entry = (byte *)dr_get_proc_address(data->handle, "NtWriteFile");
    DR_ASSERT(entry != NULL);
    dr_free_module_data(data);
    return drmgr_decode_sysnum_from_wrapper(entry);
#endif
}

static int
get_read_sysnum(void)
{
#ifdef UNIX
    return SYS_read;
#else
    byte *entry;
    module_data_t *data = dr_lookup_module_by_name("ntdll.dll");
    DR_ASSERT(data != NULL);
    entry = (byte *)dr_get_proc_address(data->handle, "NtReadFile");
    DR_ASSERT(entry != NULL);
    dr_free_module_data(data);
    return drmgr_decode_sysnum_from_wrapper(entry);
#endif
}
