#!/usr/bin/python3

from subprocess import run as _run
from sys import argv as cmdargs, stderr
from os.path import dirname, abspath, join as joinpath

def usage_and_exit(msg=None):
    if msg:
        print(msg, file=stderr)
    print(f"Usage: {argv[0]} source-type shmkey "
           "event-name data-descr event-signature ",
           "[event-name data-descr event-signature] ...",
          file=stderr)
    exit(1)

def run(cmd, *args, **kwargs):
    print("\033[32m>", " ".join(cmd), "\033[0m", file=stderr)
    return _run(cmd, *args, **kwargs)

def run_re2c(infile, outfile):
    run(["re2c", "-i", "-P", "-W", infile, "-o", outfile]).check_returncode()

def run_clang_format(infile):
    return run(["clang-format", "-i", infile]).returncode

def run_clang(src, out):
    top_dir = abspath(joinpath(dirname(cmdargs[0]), "../.."))
    core_dir = joinpath(top_dir, "core")
    shmbuf_dir = joinpath(top_dir, "shmbuf")

    include_dirs = [top_dir, core_dir]
    link_dirs = [core_dir, shmbuf_dir]
    cflags = ["-Wall", "-g"]
    ldflags = ["-lshamon-source", "-lshamon-shmbuf",
               "-lshamon-signature", "-lshamon-utils"]
    libraries = []
    run(["clang", src, "-o", out] +
        [f"-I{d}" for d in include_dirs] +
        [f"-L{d}" for d in link_dirs] +
        cflags + libraries + ldflags).check_returncode()

def msg_and_exit(msg):
    print(msg, file=stderr)
    exit(1)

def usage_and_exit(msg=None):
    if msg:
        print(msg, file=stderr)
    print(f"Usage: {argv[0]} source-type shmkey "
           "event-name data-descr event-signature ",
           "[event-name data-descr event-signature] ...",
          file=stderr)
    exit(1)

def adjust_regex(regex):
    """ Transform regex from POSIX syntax to RE2C syntax """
    return regex

def gen(source_type, shmkey, events):
    if source_type == "stdin":
        return gen_stdin(shmkey, events)
    else:
        msg_and_exit("Unknown source type")

def gen_push_simple_arg(write, n, c):
    fmt = c
    if c == "i":
        fmt = "d"
    elif c == "l":
        fmt = "ld"
    elif c == "d":
        fmt = "lf"

    write(\
    f"""
     tmp_c = *yypmatch[{n}];
     *yypmatch[{n}] = 0;
     sscanf(yypmatch[{n-1}], \"%@FMT\", &op.@ARG);
     *yypmatch[{n}] = tmp_c;
     addr = buffer_partial_push(shm, addr, &op.@ARG, sizeof(op.@ARG));
     """.replace("@FMT", fmt).replace("@ARG", c)
     )


def gen_push_event(outfile, sig):
    write = outfile.write
    for n, c in enumerate(sig, start=1):
        idx_begin = 2*n
        idx_end = 2*n+1
        if c in ('i', 'l', 'f', 'd'):
            gen_push_simple_arg(write, idx_end, c)
        elif c == 'c':
            write("addr = buffer_partial_push(shm, addr, "
                  f"yypmatch[{idx_begin}], sizeof(op.c));\n")
        elif c == 'l':
            write(f"tmp_c = *yypmatch[{idx_end}];\n")
            write(f"*yypmatch[{idx_end}] = 0;\n")
            write(f"sscanf(yypmatch[{idx_begin}], \"%l\", &op.l);\n")
            write(f"*yypmatch[{idx_end}] = tmp_c;\n")
            write("addr = buffer_partial_push(shm, addr, &op.l, sizeof(op.k));\n")
        elif c == 'L':
            write("addr = buffer_partial_push_str(shm, addr, ev.base.id, line);\n")
        elif c == 'M':
            write("addr = buffer_partial_push_str_n(shm, addr, ev.base.id, "
                  "yypmatch[0], yypmatch[1] - yypmatch[0]);\n")
        else:
            raise NotImplementedError(f"Not implemented yet: {sig}")

def gen_parse_and_push(outfile, events):
    for i in range(0, len(events), 3):
        ev_idx = int(i/3)
        ev_name = events[i]
        ev_regex = adjust_regex(events[i + 1])
        ev_sig = events[i + 2]

        parse_start =\
        f"""
        YYCURSOR = line;
        YYLIMIT = line + len;

        /*!re2c
           .*{ev_regex}.* {{
               while (!(out = buffer_start_push(shm))) {{
                   ++waiting_for_buffer;
               }}
               out->base.kind = events[{ev_idx}].kind; // FIXME: cache these
               out->base.id = ++ev.base.id;
               addr = out->args;

               char tmp_c;
        """
        parse_end =\
        f"""
               buffer_finish_push(shm);
               continue;
            }}
            *      {{ printf("NO match\\n"); goto next{ev_idx + 1}; }}
          */
        """

        write = outfile.write
        write(f"/* parsing event {ev_idx}: {ev_name}:{ev_sig} -> {ev_regex} */\n")
        write(f"if (events[{ev_idx}].kind != 0) {{")
        write(parse_start)
        gen_push_event(outfile, ev_sig)
        write(parse_end)
        write("}")
        write(f"next{ev_idx + 1}: ;")


def gen_stdin(shmkey, events):
    print(events)

    template = "regex.c.in"
    tmpoutput = "regex.c.tmp.c"
    output = "regex.c"
    events_num = int(len(events) / 3)

    evs = ", ".join(f"\"{events[i]}\", \"{events[i+2]}\""
                    for i in range(0, len(events), 3))
    source_control_src =\
    f"""
    /* Initialize the info about this source */
    struct source_control *control
        = source_control_define({events_num}, {evs});
    assert(control);
    """
    create_shared_buffer_src =\
    f"""
    /* Create the shared buffer */
    create_shared_buffer(
        \"{shmkey}\", source_control_max_event_size(control), control);
    free(control);
    """
    subs = [
        ("@EVENTS_NUM", str(events_num)),
        ("@SOURCE_CONTROL", source_control_src),
        ("@CREATE_SHARED_BUFFER", create_shared_buffer_src),
    ]

    outfile = open(tmpoutput, "w")
    infile = open(template, "r")
    for line in infile:
        # do substitution
        if "@" in line:
            if line.strip() == "@PARSE_AND_PUSH":
                gen_parse_and_push(outfile, events)
                continue
            for s in subs:
                line = line.replace(s[0], s[1])
        outfile.write(line)
    infile.close()
    outfile.close()

    run_re2c(tmpoutput, output)

    return output

def main(argv):
    """
    source-type shmkey event-name data-descr event-signature [event-name
    data-descr event-signature] ...

    data-descr is usually a regular expression
    """
    if len(argv) < 6:
        usage_and_exit()

    source_type = argv[1]
    shmkey = argv[2]
    events = argv[3:]

    if shmkey[0] != '/':
        usage_and_exit("shmkey must start with /")
    if len(events) % 3 != 0:
        usage_and_exit()

    src = gen(source_type, shmkey, argv[3:])
    run_clang_format(src)
    run_clang(src, "source")

if __name__ == "__main__":
    main(cmdargs)
