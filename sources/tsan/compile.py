#!/usr/bin/env python3

import sys
from subprocess import run, PIPE
from os.path import dirname, abspath, basename

DIR = abspath(dirname(sys.argv[0]))
SHAMONDIR = f"{DIR}/../.."
LLVM_PASS_DIR = f"{DIR}/../llvm"
CFLAGS = [
    "-DDEBUG_STDOUT",  "-std=c11"
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

def get_build_type():
    config = f"{SHAMONDIR}/shamonConfig.cmake"
    with open(config, "r") as f:
        for line in f:
            if line.startswith("set( SHAMON_BUILD_TYPE"):
                return line.split()[2]
    return None

def get_compiler():
    config = f"{SHAMONDIR}/shamonConfig.cmake"
    with open(config, "r") as f:
        for line in f:
            if line.startswith("set( SHAMON_C_COMPILER"):
                return line.split()[2]
    return None

def get_llvm_version():
    proc = run(["clang", "--version"], stdout=PIPE)
    ver = proc.stdout.split()[2].split(b'-')[0].split(b'.')
    if len(ver) == 3:
        return int(ver[0]), int(ver[1]), int(ver[2])
    return None

class CompileOptions:
    def __init__(self):
        self.files = []
        self.files_noinst = []
        self.cc = "clang"
        self.llvm_cc = "clang"
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
        elif argv[i] == "-llvm-cc":
            i += 1
            opts.llvm_cc = argv[i]
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

    build_type = get_build_type()
    if build_type is None or build_type == "Release":
        CFLAGS.extend(("-g3", "-O3", "-flto", "-DNDEBUG",
                       "-fno-fat-lto-objects", ))
    else:
        CFLAGS.append("-g")

    if build_type == "Release" and basename(get_compiler()) != "clang":
        print("WARNING: Shamon was build in Release mode but not with clang. "\
              "It may cause troubles with linking.")

    opt_args = []
    llvm_version = get_llvm_version()
    if llvm_version is None or llvm_version[0] > 12:
        opt_args.append("-enable-new-pm=0")

    output = opts.output
    compiled_files = [f"{file}.bc" for file in opts.files]
    for f, out in zip(opts.files, compiled_files):
        cmd(
            [opts.llvm_cc, "-emit-llvm", "-c", "-fsanitize=thread", "-O0", "-o", f"{out}", f]
            + CFLAGS
            + opts.cflags
        )
    cmd([link, "-o", f"{output}.tmp2.bc"] + compiled_files + opts.link_and_instrument)

    cmd(
        [
            opt,
            "-load",
            f"{LLVM_PASS_DIR}/race-instrumentation.so",
            "-vamos-race-instrumentation",
            f"{output}.tmp2.bc",
            "-o",
            f"{output}.tmp3.bc",
        ] + opt_args
    )

    cmd(
        [
            opts.llvm_cc,
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
            [opts.llvm_cc, "-emit-llvm", "-c", "-g", "-o", f"{out}", f]
            + CFLAGS
            + opts.cflags
        )
    cmd(
        [link, f"{DIR}/tsan_impl.bc", f"{output}.tmp3.bc", "-o", f"{output}.tmp4.bc"]
        + opts.link
        + compiled_files_link
    )

    cmd([opt, "-O3", f"{output}.tmp4.bc", "-o", f"{output}.tmp5.bc"])
    cmd([opts.llvm_cc, f"{output}.tmp5.bc", "-c", "-o", f"{output}.tmp5.o"]
        + opts.link_asm
        + opts.cflags
        + CFLAGS)
    cmd(
        [opts.cc, "-pthread", f"{output}.tmp5.o", "-o", output]
        + opts.cflags
        + CFLAGS
        + SHAMON_LIBS
    )


if __name__ == "__main__":
    main(sys.argv)
    exit(0)
