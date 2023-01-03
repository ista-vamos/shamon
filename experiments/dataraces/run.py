#!/usr/bin/env python3

import sys
from subprocess import run, Popen, PIPE, DEVNULL, TimeoutExpired
from os.path import dirname, abspath, basename

from randomharness import gen_harness

opt = "opt"
link = "llvm-link"

# we expect output like this:
# 0.01user 0.00system 0:00.03elapsed 48%CPU (0avgtext+0avgdata 25772maxresident)k
# 0inputs+0outputs (0major+1946minor)pagefaults 0swaps
timecmd = "/bin/time"

DIR = abspath(dirname(sys.argv[0]))
TIMEOUT = 5
REPEAT_NUM = 10


def cmd(args):
    print("> ", " ".join(args))
    run(args, check=True)


# def parse_yml_input(path):
#     try:
#         from yaml import safe_load as yaml_safe_load, YAMLError
#     except ImportError:
#         warn("Cannot import from YAML package")
#         return None

#     with open(path, "r") as stream:
#         try:
#             spec = yaml_safe_load(stream)
#         except YAMLError as exc:
#             warn(exc)
#             return None
#     return spec


def parse_time(tm):
    parts = tm.split(":")
    assert len(parts) == 2
    return 60 * int(parts[0]) + float(parts[1])


def compile_file(infile):
    harness = f"{DIR}/harness.tmp.c"

    # generate harness
    with open(harness, "w") as out:
        gen_harness(out, 1024)

    # compile TSAN
    cmd(
        [
            "clang",
            infile,
            harness,
            f"{DIR}/svcomp_atomic.c",
            "-g",
            "-fsanitize=thread",
            "-O3",
            "-o",
            "a.tsan.out",
        ]
    )

    # compile HELGRIND
    cmd(
        [
            "gcc",
            infile,
            harness,
            f"{DIR}/svcomp_atomic.c",
            "-g",
            "-O3",
            "-o",
            "a.helgrind.out",
        ]
    )

    # compile VAMOS
    cmd(
        [
            f"{DIR}/../../sources/tsan/compile.py",
            infile,
            "-noinst",
            harness,
            "-noinst",
            f"{DIR}/svcomp_atomic.c",
            "-o",
            "a.vamos.out",
        ]
    )


def run_once():
    result = {}

    # ---- run TSAN
    result.setdefault("tsan", {})["verdict"] = "no"
    try:
        proc = Popen([timecmd, "./a.tsan.out"], stderr=PIPE, stdout=DEVNULL)
        _, err = proc.communicate(timeout=TIMEOUT)
        for line in err.splitlines():
            if b"WARNING: ThreadSanitizer: data race" in line:
                result["tsan"]["verdict"] = "yes"
            if b"elapsed" in line and b"maxresident" in line:
                words = line.split()
                result["tsan"]["usertime"] = float(words[0][:-4])
                result["tsan"]["systime"] = float(words[1][:-6])
                result["tsan"]["time"] = parse_time(words[2][:-7].decode("ascii"))
                result["tsan"]["maxmem"] = int(
                    words[5][0 : words[5].find(b"maxresident")]
                )
    except TimeoutExpired:
        result["tsan"]["verdict"] = "to"
        proc.kill()

    # --- run HELGRIND
    result.setdefault("helgrind", {})["verdict"] = "no"
    try:
        proc = Popen(
            [timecmd, "valgrind", "--tool=helgrind", "./a.helgrind.out"],
            stderr=PIPE,
            stdout=DEVNULL,
        )
        _, err = proc.communicate(timeout=TIMEOUT)
        for line in err.splitlines():
            if b"Possible data race" in line:
                result["helgrind"]["verdict"] = "yes"
            if b"elapsed" in line and b"maxresident" in line:
                words = line.split()
                result["helgrind"]["usertime"] = float(words[0][:-4])
                result["helgrind"]["systime"] = float(words[1][:-6])
                result["helgrind"]["time"] = parse_time(words[2][:-7].decode("ascii"))
                result["helgrind"]["maxmem"] = int(
                    words[5][0 : words[5].find(b"maxresident")]
                )
    except TimeoutExpired:
        result["helgrind"]["verdict"] = "to"
        proc.kill()

    # --- run VAMOS
    result.setdefault("vamos", {})["verdict"] = "no"
    try:
        proc = Popen([timecmd, "./a.vamos.out"], stderr=PIPE, stdout=DEVNULL)
        mon = Popen(["./monitor", "Program:generic:/vrd"], stderr=PIPE, stdout=DEVNULL)
        _, err_proc = proc.communicate(timeout=TIMEOUT)
        _, err_mon = mon.communicate(timeout=TIMEOUT)

        for line in err_mon.splitlines():
            if line.startswith(b"Found data race"):
                result["vamos"]["verdict"] = "yes"
            elif b"Dropped" in line and b"holes" in line:
                words = line.split()
                result["vamos"]["dropped"] = int(words[1])
                result["vamos"]["holes"] = int(words[4])
        for line in err_proc.splitlines():
            if b"elapsed" in line and b"maxresident" in line:
                words = line.split()
                result["vamos"]["usertime"] = float(words[0][:-4])
                result["vamos"]["systime"] = float(words[1][:-6])
                result["vamos"]["time"] = parse_time(words[2][:-7].decode("ascii"))
                result["vamos"]["maxmem"] = int(
                    words[5][0 : words[5].find(b"maxresident")]
                )
            if b"info: number of emitted events" in line:
                result["vamos"]["eventsnum"] = int(line.split()[5])
    except TimeoutExpired:
        result["vamos"]["verdict"] = "to"
    finally:
        proc.kill()
        mon.kill()

    # the output is:
    # benchmark,
    # tsan-{verdict, usertime, systime, time, maxmem},
    # helgrind-{verdict, usertime, systime, time, maxmem},
    # vamos-{verdict, usertime, systime, time, maxmem}, vamos-{eventsnum, dropped, holes}
    ret = []
    for tool in "tsan", "helgrind", "vamos":
        res = result[tool]
        ret += [
            res.get(k) for k in ("verdict", "usertime", "systime", "time", "maxmem")
        ]
        if tool == "vamos":
            ret += [res.get(k) for k in ("eventsnum", "dropped", "holes")]

    return ret


def get_stats(results):
    assert len(results) > 0

    def tonum(s):
        if s in ("yes", "no", "to"):
            return 1 if s == "yes" else 0
        assert s is None or isinstance(s, (int, float)), f"{s}: {type(s)}"
        return s

    retlen = len(results[0])
    ret = [0] * retlen

    for i in range(retlen):
        for result in results:
            ret[i] += tonum(result[i])

    return [x / len(results) for x in ret]


def run_rep(infile):
    compile_file(infile)
    results = []
    i = 0
    n = 0
    while True:
        i += 1
        if i > 2 * REPEAT_NUM:
            raise RuntimeError("Failed measuring")
        result = run_once()
        print("#", basename(infile), ",".join((str(r) for r in result)))
        if "to" in result or None in result:
            continue
        assert len(result) == 18
        results.append(result)
        n += 1
        if n == REPEAT_NUM:
            break

    stats = get_stats(results)
    print("RESULT", basename(infile), ",".join((f"{r:.2f}" for r in stats)))
    return stats

def main(argv):
    if len(argv) != 2:
        print("Usage: ./run.py file.c")
        exit(1)

    run_rep(argv[1])

if __name__ == "__main__":
    main(sys.argv)
    exit(0)
