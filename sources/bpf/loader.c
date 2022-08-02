#include <bpf/libbpf.h>
#include <stdio.h>
#include <unistd.h>

struct event {
  __u32 pid;
  char filename[16];
};

static int buf_process_sample(void *ctx, void *data, size_t len) {
  struct event *evt = (struct event *)data;
  printf("%d %s\n", evt->pid, evt->filename);

  return 0;
}

int main(int argc, char *argv[]) {
  const char *file = "program.bpf";
  struct bpf_object *obj;
  int prog_fd = -1;
  int buffer_map_fd = -1;
  struct  bpf_program *prog;

  bpf_prog_load(file, BPF_PROG_TYPE_TRACEPOINT, &obj, &prog_fd);

  buffer_map_fd = bpf_object__find_map_fd_by_name(obj, "buffer");

  struct ring_buffer *ring_buffer;

  ring_buffer = ring_buffer__new(buffer_map_fd, buf_process_sample, NULL, NULL);

  if(!ring_buffer) {
    fprintf(stderr, "failed to create ring buffer\n");
    return 1;
  }

  prog = bpf_object__find_program_by_title(obj, "tracepoint/syscalls/sys_enter_execve");
  if (!prog) {
    fprintf(stderr, "failed to find tracepoint\n");
    return 1;
  }

  bpf_program__attach_tracepoint(prog, "syscalls", "sys_enter_execve");

  while(1) {
    ring_buffer__consume(ring_buffer);
    sleep(1);
  }

  return 0;
}

