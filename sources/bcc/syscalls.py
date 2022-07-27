#!/usr/bin/python3

from sys import stderr, path as importpath, argv
from os.path import abspath
from time import sleep
from re import compile as regex_compile

from bcc import BPF

importpath.append(abspath("../../python"))
from shamon import *

submit_next_event=\
r"""
    event = buffer.ringbuf_reserve(sizeof(struct event));
    if (!event) {
        bpf_trace_printk("FAILED RESERVING SLOT IN BUFFER");
        return 1;
    }
    len = (args->count - off > BUF_SIZE) ? BUF_SIZE : args->count - off;
    ret = bpf_probe_read_user(event->buf, len, user_buf + off);
    if (ret != 0) {
        bpf_trace_printk("FAILED READING USER STRING");
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

"""

src = r"""
BPF_RINGBUF_OUTPUT(buffer, 1 << 6);

#define BUF_SIZE 255
struct event {
    char buf[BUF_SIZE];
    int count;
    int len;
    int off;
    int fd;
};

TRACEPOINT_PROBE(syscalls, sys_enter_write) {
    int ret;
    unsigned int len, off = 0;

    if (args->fd != 1) {
        return 0; // not interested
    }

    if (args->count == 0)
        return 0;

    struct event *event;
    const char *user_buf = args->buf;

    @SUBMIT_EVENTS

    /* handle the left text if there is any
       (if we haven't terminated the program in SUBMIT_EVENTS) */
    event = buffer.ringbuf_reserve(sizeof(struct event));
    if (!event) {
        bpf_trace_printk("FAILED RESERVING SLOT (2) IN BUFFER\\n");
        return 1;
    }

    /* Tell monitor that the text was too long */
    event->buf[0] = 0;
    event->count = args->count - off;
    event->fd = args->fd;
    event->len = -1;
    event->off = off;
    buffer.ringbuf_submit(event, 0);
    bpf_trace_printk("TOO LONG\n", 10);

    return 0;
}
""".replace("@SUBMIT_EVENTS", 18*submit_next_event)

b = BPF(text=src)
get_data = b['buffer'].event
partial = b""

def printstderr(x): print(x, file=stderr)

shmkey = argv[1]
event_name = argv[2]
regex = argv[3]
signature = argv[4]

event_size = signature_get_size(signature) + 16  # + kind + id
print("Event size: ", event_size)

shmbuf = create_shared_buffer(shmkey, event_size, f"{event_name}:{signature}")

kind = 2     # FIXME
text_long_kind = 3 # FIXME
evid = 1

waiting_for_buffer = 0

regexc = regex_compile(regex)

def parse_lines(buff):
    global evid
    for line in buff.splitlines():
        printstderr(f"> \033[33;2m`{line}`\033[0m")
        m = regexc.match(line)
        if not m:
            continue

        n = 1

        # start a write into the buffer
        addr = buffer_start_push(shmbuf)
        while not addr:
            addr = buffer_start_push(shmbuf)
            global waiting_for_buffer
            waiting_for_buffer += 1
        # push the base data (kind and event ID)
        addr = buffer_partial_push(shmbuf, addr, kind.to_bytes(8, "little"), 8)
        addr = buffer_partial_push(shmbuf, addr, evid.to_bytes(8, "little"), 8)

        for o in signature:
            printstderr("match: " + str(m[n]))
            if o == 'i':
                addr = buffer_partial_push(shmbuf, addr,
                                           int(m[n]).to_bytes(4, "little"), 4)
            elif o == 'l':
                addr = buffer_partial_push(shmbuf, addr,
                                           int(m[n]).to_bytes(8, "little"), 8)
            elif o == 'S':
                addr = buffer_partial_push_str(shmbuf, addr, evid, m[n])
            elif o == 'M':
                addr = buffer_partial_push_str(shmbuf, addr, evid, m[0])
            else:
                raise NotImplementedError("Not implemented signature op")
            n += 1

        buffer_finish_push(shmbuf)
        printstderr(f"sent ev `{evid}`")
        evid += 1

def callback(ctx, data, size):
    global partial

    event = get_data(data)
    s = event.buf
    printstderr(f"\033[34;1m[fd: {event.fd}, count: {event.count}, "\
                f"off: {event.off}, len: {event.len}]\033[0m")
    if event.len == event.count:
        buff = None
        if partial:
            buff = partial + s
            partial = b""
        else:
            buff = s

        assert buff is not None
        parse_lines(buff.decode("utf-8", "ignore"))
    elif event.len < event.count:
        if event.len == -1:
            printstderr(partial + s)
            printstderr(f"... TEXT TOO LONG, DROPPED {event.count} CHARS")
            partial = b""
            raise NotImplementedError("Unhandled situation")
        else:
            partial += s
    else:
        raise NotImplementedError("Unhandled situation")


b['buffer'].open_ring_buffer(callback)

try:
    while 1:
        b.ring_buffer_consume()
        sleep(0.01)
except KeyboardInterrupt:
    exit(0)
finally:
    print(f"Waited for buffer {waiting_for_buffer} cycles")
    destroy_shared_buffer(shmbuf)
