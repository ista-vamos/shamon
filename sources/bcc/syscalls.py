#!/usr/bin/python3

from sys import stderr
import time

from bcc import BPF

src = r"""
BPF_RINGBUF_OUTPUT(buffer, 1 << 4);

#define BUF_SIZE 255
struct event {
    char buf[BUF_SIZE];
    int count;
    int len;
    int off;
    int fd;
};

TRACEPOINT_PROBE(syscalls, sys_enter_write) {
    int len;
    unsigned int max_read_len, off = 0;

    if (args->fd != 1) {
        return 0; // not interested
    }

    if (args->count == 0)
        return 0;
    const char *user_buf = args->buf;

    struct event *event = buffer.ringbuf_reserve(sizeof(struct event));
    if (!event) {
        bpf_trace_printk("FAILED RESERVING SLOT IN BUFFER\\n");
        return 1;
    }

    /* 1st PART */
    max_read_len = (args->count > BUF_SIZE) ? BUF_SIZE : args->count;
    len = bpf_probe_read_user_str(event->buf, max_read_len, user_buf);
    if (len < 0) {
        bpf_trace_printk("FAILED READING USER STRING\\n");
        buffer.ringbuf_discard(event, 0);
        return 1;
    } else {
        bpf_trace_printk("SUBMIT EVENT\\n");
        event->count = args->count;
        event->fd = args->fd;
        event->len = len;
        event->off = 0;
        buffer.ringbuf_submit(event, 0);
    }
    off += len;
    if (off >= args->count)
        return 0;

    /* 2nd PART */
    event = buffer.ringbuf_reserve(sizeof(struct event));
    if (!event) {
        bpf_trace_printk("FAILED RESERVING SLOT (2) IN BUFFER\\n");
        return 1;
    }
    max_read_len = (args->count - off > BUF_SIZE) ? BUF_SIZE : args->count - off;
    len = bpf_probe_read_user_str(event->buf, max_read_len, user_buf + off);
    if (len < 0) {
        bpf_trace_printk("FAILED READING USER STRING\\n");
        buffer.ringbuf_discard(event, 0);
        return 1;
    } else {
        bpf_trace_printk("SUBMIT EVENT\\n");
        event->count = args->count - off;
        event->fd = args->fd;
        event->len = len;
        event->off = off;
        buffer.ringbuf_submit(event, 0);
    }
    off += len;
    if (off >= args->count)
        return 0;

    /* 3rd PART */
    event = buffer.ringbuf_reserve(sizeof(struct event));
    if (!event) {
        bpf_trace_printk("FAILED RESERVING SLOT (2) IN BUFFER\\n");
        return 1;
    }
    max_read_len = (args->count - off > BUF_SIZE) ? BUF_SIZE : args->count - off;
    len = bpf_probe_read_user_str(event->buf, max_read_len, user_buf + off);
    if (len < 0) {
        bpf_trace_printk("FAILED READING USER STRING\\n");
        buffer.ringbuf_discard(event, 0);
        return 1;
    } else {
        bpf_trace_printk("SUBMIT EVENT\\n");
        event->count = args->count - off;
        event->fd = args->fd;
        event->len = len;
        event->off = off;
        buffer.ringbuf_submit(event, 0);
    }
    off += len;
    if (off >= args->count)
        return 0;

    /* 4th PART */
    event = buffer.ringbuf_reserve(sizeof(struct event));
    if (!event) {
        bpf_trace_printk("FAILED RESERVING SLOT (2) IN BUFFER\\n");
        return 1;
    }
    max_read_len = (args->count - off > BUF_SIZE) ? BUF_SIZE : args->count - off;
    len = bpf_probe_read_user_str(event->buf, max_read_len, user_buf + off);
    if (len < 0) {
        bpf_trace_printk("FAILED READING USER STRING\\n");
        buffer.ringbuf_discard(event, 0);
        return 1;
    } else {
        bpf_trace_printk("SUBMIT EVENT\\n");
        event->count = args->count - off;
        event->fd = args->fd;
        event->len = len;
        event->off = off;
        buffer.ringbuf_submit(event, 0);
    }
    off += len;
    if (off >= args->count)
        return 0;

    /* 5th PART */
    event = buffer.ringbuf_reserve(sizeof(struct event));
    if (!event) {
        bpf_trace_printk("FAILED RESERVING SLOT (2) IN BUFFER\\n");
        return 1;
    }
    max_read_len = (args->count - off > BUF_SIZE) ? BUF_SIZE : args->count - off;
    len = bpf_probe_read_user_str(event->buf, max_read_len, user_buf + off);
    if (len < 0) {
        bpf_trace_printk("FAILED READING USER STRING\\n");
        buffer.ringbuf_discard(event, 0);
        return 1;
    } else {
        bpf_trace_printk("SUBMIT EVENT\\n");
        event->count = args->count - off;
        event->fd = args->fd;
        event->len = len;
        event->off = off;
        buffer.ringbuf_submit(event, 0);
    }
    off += len;
    if (off >= args->count)
        return 0;

    bpf_trace_printk("EVENT TOO LONG\\n");
    return 0;
}
"""


b = BPF(text=src)
get_data = b['buffer'].event

def callback(ctx, data, size):
    event = get_data(data)
    s = event.buf.decode('utf-8', 'ignore')
    print(f"\033[34;1m[fd: {event.fd}, count: {event.count}, off: {event.off}, len: {event.len}]\033[0m:",
          file=stderr)
    print(f"'{s}'", file=stderr)

b['buffer'].open_ring_buffer(callback)

try:
    while 1:
        b.ring_buffer_consume()
except KeyboardInterrupt:
    exit()
