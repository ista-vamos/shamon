#include <unistd.h>
#include <unistd.h>
#include <bpf/bpf.h>
#include "syswrite.h"
#include "syswrite.skel.h"
#include "errno_helpers.h"
#include "btf_helpers.h"
#include "trace_helpers.h"

#define warn(...) fprintf(stderr, __VA_ARGS__)

/*
static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
	if (level == LIBBPF_DEBUG && !env.verbose)
		return 0;

	return vfprintf(stderr, format, args);
}

void bump_memlock_rlimit(void)
{
    struct rlimit rlim_new = {
        .rlim_cur	= RLIM_INFINITY,
        .rlim_max	= RLIM_INFINITY,
    };

    if (setrlimit(RLIMIT_MEMLOCK, &rlim_new)) {
        fprintf(stderr, "Failed to increase RLIMIT_MEMLOCK limit!\n");
        exit(1);
    }
}
*/

int handle_event(void *ctx, void *data, size_t data_sz)
{
    const struct event *e = data;

    if (e->len == -2) {
        fprintf(stderr, "\033[31mDROPPED %d\033[0m\n", e->count);
        return 0;
    }
    /*
    fprintf(stderr, "fd: %d, len: %d, off: %d, count: %d, str:\n\033[34m'%*s'\033[0m\n",
            e->fd, e->len, e->off, e->count, e->len, e->buf);
            */

    return 0;
}

int main(int argc, char **argv)
{
	LIBBPF_OPTS(bpf_object_open_opts, open_opts);
	struct syswrite_bpf *obj;
	int err;

	libbpf_set_strict_mode(LIBBPF_STRICT_ALL);
    //libbpf_set_print(libbpf_print_fn);

	err = ensure_core_btf(&open_opts);
	if (err) {
		fprintf(stderr, "failed to fetch necessary BTF for CO-RE: %s\n", strerror(-err));
		return 1;
	}

	obj = syswrite_bpf__open_opts(&open_opts);
	if (!obj) {
		warn("failed to open BPF object\n");
		err = 1;
        goto cleanup_core;
	}

    /*
	if (env.pid)
		obj->rodata->filter_pid = env.pid;
    */
	err = syswrite_bpf__load(obj);
	if (err) {
		warn("failed to load BPF object: %s\n", strerror(-err));
		goto cleanup_obj;
	}

    obj->links.sys_write = bpf_program__attach(obj->progs.sys_write);
    if (!obj->links.sys_write) {
		err = -errno;
        warn("failed to attach sys_write program: %s\n", strerror(-err));
		goto cleanup_obj;
	}

    struct ring_buffer *buffer = ring_buffer__new(bpf_map__fd(obj->maps.buffer), handle_event, NULL, NULL);
    if (!buffer) {
        warn("Failed to create ring buffer\n");
        goto cleanup_obj;
    }

    printf("Tracing write syscalls...\n");
    while (1) {
        err = ring_buffer__consume(buffer);
        //err = ring_buffer__poll(buffer, 0);
                                //100 /* timeout in ms */);
        if (err < 0)
            perror("polling");
	}

    printf("Cleaning up...\n");
    ring_buffer__free(buffer);

cleanup_obj:
	syswrite_bpf__destroy(obj);
cleanup_core:
	cleanup_core_btf(&open_opts);

	return err != 0;
}
