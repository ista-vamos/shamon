#include <linux/types.h>

#include <bpf/bpf_helpers.h>
#include <linux/bpf.h>

struct event {
  __u32 pid;
  char filename[16];
};

struct bpf_map_def SEC("maps") buffer = {
    .type = BPF_MAP_TYPE_RINGBUF,
    .max_entries = 4096 * 64,
};

struct trace_entry {
  short unsigned int type;
  unsigned char flags;
  unsigned char preempt_count;
  int pid;
};

struct trace_event_raw_sys_enter {
  struct trace_entry ent;
  long int id;
  long unsigned int args[6];
  char __data[0];
};


SEC("tracepoint/syscalls/sys_enter_execve")
int sys_enter_execve(struct trace_event_raw_sys_enter *ctx) {
  __u32 pid = bpf_get_current_pid_tgid() >> 32;
  struct event *event = bpf_ringbuf_reserve(&buffer, sizeof(struct event), 0);
  if (!event) {
    return 1;
  }
  event->pid = pid;
  bpf_probe_read_user_str(event->filename, sizeof(event->filename),
                          (const char *)ctx->args[0]);

  bpf_ringbuf_submit(event, 0);

  return 0;
}

char _license[] SEC("license") = "GPL";
