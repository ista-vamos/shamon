#!/usr/bin/env python3

import sys
from subprocess import run, Popen, PIPE, DEVNULL, TimeoutExpired
from os.path import dirname, abspath, basename

from randomharness import gen_harness

opt = "opt"
link = "llvm-link"

DIR = abspath(dirname(sys.argv[0]))
TIMEOUT=5

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



def main(argv):

    if len(argv) != 2:
        print("Usage: ./run.py file.c")
        exit(1)

    infile = argv[1]
    harness = f"{DIR}/harness.tmp.c"

    # generate harness
    with open(harness, "w") as out:
        gen_harness(out, 1024)

    # compile TSAN
    cmd(["clang", infile, harness,  f"{DIR}/svcomp_atomic.c", "-g", "-fsanitize=thread", "-O3", "-o", "a.tsan.out"])

    # compile HELGRIND
    cmd(["gcc", infile, harness,  f"{DIR}/svcomp_atomic.c", "-g", "-O3", "-o", "a.helgrind.out"])

    # compile VAMOS
    cmd([f"{DIR}/../../sources/tsan/compile.py", infile, "-noinst", harness, "-noinst", f"{DIR}/svcomp_atomic.c", "-o", "a.vamos.out"])

    result = {}

    # ---- run TSAN
    result["tsan"] = "no"
    try:
        proc = Popen(["./a.tsan.out"], stderr=PIPE, stdout=DEVNULL)
        _, err = proc.communicate(timeout=TIMEOUT)
        for line in err.splitlines():
            if b'WARNING: ThreadSanitizer: data race' in line:
                result["tsan"] = "yes"
    except TimeoutExpired:
        result["tsan"] = "to"
        proc.kill()

    # --- run HELGRIND
    result["helgrind"] = "no"
    try:
        proc = Popen(["valgrind", "--tool=helgrind", "./a.helgrind.out"], stderr=PIPE, stdout=DEVNULL)
        _, err = proc.communicate(timeout=TIMEOUT)
        for line in err.splitlines():
            if b'Possible data race' in line:
                result["helgrind"] = "yes"
    except TimeoutExpired:
        result["helgrind"] = "to"
        proc.kill()

    # --- run VAMOS
    result["vamos"] = "no"
    try:
        proc = Popen(["./a.vamos.out"], stderr=PIPE, stdout=DEVNULL)
        mon  = Popen(["./monitor", "Program:generic:/vrd"], stderr=PIPE, stdout=DEVNULL)
        _, err_proc = proc.communicate(timeout=TIMEOUT)
        _, err_mon  = mon.communicate(timeout=TIMEOUT)

        for line in err_mon.splitlines():
            if line.startswith(b'Found data race'):
                print(line)
                result["vamos"] = "yes"
    except TimeoutExpired:
        result["vamos"] = "to"
    finally:
        proc.kill()
        mon.kill()

    print("RESULT", basename(infile), end="")
    for tool in "tsan", "helgrind", "vamos":
        res = result.get(tool)
        print(f", {res}", end="")
    print()

if __name__ == "__main__":
    main(sys.argv)
    exit(0)
