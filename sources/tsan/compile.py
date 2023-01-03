#!/usr/bin/env python3

import sys
from subprocess import run
from os.path import dirname, abspath

DIR = abspath(dirname(sys.argv[0]))
SHAMONDIR = f"{DIR}/../.."
LLVM_PASS_DIR = f"{DIR}/../llvm"
CFLAGS = [
    "-DDEBUG_STDOUT",
    "-g",  # "-O3"
]
SHAMON_INCLUDES = [f"-I{DIR}/../../"]
SHAMON_LIBS = [
    f"{SHAMONDIR}/core/libshamon-arbiter.a",
    f"{SHAMONDIR}/core/libshamon-stream.a",
    f"{SHAMONDIR}/core/libshamon-parallel-queue.a",
    f"{SHAMONDIR}/shmbuf/libshamon-shmbuf.a",
    f"{SHAMONDIR}/core/libshamon-list.a",
    f"{SHAMONDIR}/core/libshamon-source.a",
    f"{SHAMONDIR}/core/libshamon-signature.a",
    f"{SHAMONDIR}/core/libshamon-event.a",
    f"{SHAMONDIR}/streams/libshamon-streams.a",
]

opt = "opt"
link = "llvm-link"


class CompileOptions:
    def __init__(self):
        self.files = []
        self.files_noinst = []
        self.cc = "clang"
        self.output = "a.out"
        self.cflags = []
        self.link = []
        self.link_asm = []
        self.link_and_instrument = []


def get_opts(argv):
    opts = CompileOptions()

    i = 1
    while i < len(argv):
        if argv[i] == "-o":
            i += 1
            opts.output = argv[i]
        elif argv[i] == "-cc":
            i += 1
            opts.cc = argv[i]
        elif argv[i] == "-li":
            i += 1
            opts.link_and_instrument.append(argv[i])
        elif argv[i] == "-l":
            i += 1
            opts.link.append(argv[i])
        elif argv[i] == "-I":
            i += 1
            opts.cflags.append(f"-I{argv[i]}")
        elif argv[i] == "-noinst":
            i += 1
            opts.files_noinst.append(argv[i])
        elif argv[i] == "-omp":
            i += 1
            opts.link_and_instrument.append(argv[i])
            opts.cflags.append("-fopenmp")
        else:
            s = argv[i]
            if any((s.endswith(suffix) for suffix in (".c", ".cpp", ".i"))):
                opts.files.append(s)
            elif any((s.endswith(suffix) for suffix in (".bc", ".ll"))):
                opts.link_and_instrument.append(s)
            elif s.endswith(".S"):
                opts.link_asm.append(s)
            else:
                opts.cflags.append(s)
        i += 1
    return opts


def cmd(args):
    print("> ", " ".join(args))
    run(args, check=True)


def main(argv):
    opts = get_opts(argv)
    assert opts.files, "No input files given"

    output = opts.output
    compiled_files = [f"{file}.bc" for file in opts.files]
    for f, out in zip(opts.files, compiled_files):
        cmd(
            [opts.cc, "-emit-llvm", "-c", "-fsanitize=thread", "-O0", "-o", f"{out}", f]
            + CFLAGS
            + opts.cflags
        )
    cmd([link, "-o", f"{output}.tmp2.bc"] + compiled_files + opts.link_and_instrument)

    cmd(
        [
            opt,
            "-enable-new-pm=0",
            "-load",
            f"{LLVM_PASS_DIR}/race-instrumentation.so",
            "-vamos-race-instrumentation",
            f"{output}.tmp2.bc",
            "-o",
            f"{output}.tmp3.bc",
        ]
    )

    cmd(
        [
            opts.cc,
            "-std=c11",
            "-emit-llvm",
            "-c",
            f"{DIR}/tsan_impl.c",
            "-o",
            f"{DIR}/tsan_impl.bc",
        ]
        + CFLAGS
        + SHAMON_INCLUDES
    )
    compiled_files_link = [f"{file}.bc" for file in opts.files_noinst]
    for f, out in zip(opts.files_noinst, compiled_files_link):
        cmd(
            [opts.cc, "-emit-llvm", "-c", "-g", "-o", f"{out}", f]
            + CFLAGS
            + opts.cflags
        )
    cmd(
        [link, f"{DIR}/tsan_impl.bc", f"{output}.tmp3.bc", "-o", f"{output}.tmp4.bc"]
        + opts.link
        + compiled_files_link
    )

    cmd([opt, "-O3", f"{output}.tmp4.bc", "-o", f"{output}.tmp5.bc"])
    cmd(
        [opts.cc, "-pthread", f"{output}.tmp5.bc", "-o", output]
        + opts.link_asm
        + opts.cflags
        + CFLAGS
        + SHAMON_LIBS
    )


if __name__ == "__main__":
    main(sys.argv)
    exit(0)
