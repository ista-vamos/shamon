#!/usr/bin/env python3

import sys
from subprocess import run
from os.path import dirname, abspath

DIR=dirname(sys.argv[0])
SHAMONDIR=f"{DIR}/../.."
LLVM_PASS_DIR=f"{DIR}/../llvm"
CFLAGS=[
    "-DDEBUG_STDOUT", "-g", #"-O3"
    ]
SHAMON_INCLUDES=[f"-I{DIR}/../../"]
SHAMON_LIBS=[
    f"{SHAMONDIR}/core/libshamon-arbiter.a",
    f"{SHAMONDIR}/shmbuf/libshamon-shmbuf.a",
    f"{SHAMONDIR}/core/libshamon-utils.a",
    f"{SHAMONDIR}/core/libshamon-stream.a",
    f"{SHAMONDIR}/core/libshamon-parallel-queue.a",
    f"{SHAMONDIR}/core/signatures.c",
    f"{SHAMONDIR}/streams/libshamon-streams.a"
]

opt = "opt"
link = "llvm-link"


def get_args(argv):
    prog=None
    cc="clang"
    output="a.out"
    flags=[]
    tolink=[]
    openmp = None

    i = 1
    while i < len(argv):
        if argv[i] == "-o":
            i += 1
            output = argv[i]
        elif argv[i] == "-cc":
            i += 1
            cc = argv[i]
        elif argv[i] == "-l":
            i += 1
            tolink.append(argv[i])
        elif argv[i] == "-omp":
            i += 1
            openmp = argv[i]
        else:
            if prog is None:
                prog = argv[i]
            else:
                flags.append(argv[i])
        i += 1
    return prog, cc, abspath(output), flags, tolink, openmp

def cmd(args):
    print("> ", " ".join(args))
    run(args, check=True)

def main(argv):
    prog, cc, output, flags, tolink, omp = get_args(argv)
    flags = CFLAGS + flags

   #print(f"Compiler: {cc}")
   #print(f"Program: {prog}")
   #print(f"Flags: {flags}")
   #print(f"Output: {output}")

    cmd([cc, "-emit-llvm", "-c", "-fsanitize=thread", "-O0", "-o", f"{output}.tmp1.bc", prog] + flags)

    cmd([opt, "-enable-new-pm=0", "-load",  f"{LLVM_PASS_DIR}/race-instrumentation.so",
              "-vamos-race-instrumentation", f"{output}.tmp1.bc", "-o", f"{output}.tmp2.bc"])
    cmd([cc, "-std=c11", "-emit-llvm", "-c", f"{DIR}/tsan_impl.c", "-o", f"{DIR}/tsan_impl.bc"] + flags + SHAMON_INCLUDES)
    cmd([link, f"{DIR}/tsan_impl.bc", f"{output}.tmp2.bc", "-o", f"{output}.tmp3.bc"] + tolink)
    cmd([opt, "-O3", f"{output}.tmp3.bc", "-o", f"{output}.tmp4.bc"])
    cmd([cc, "-pthread", f"{output}.tmp4.bc", "-o", output] + flags + SHAMON_LIBS)



if __name__ == "__main__":
    main(sys.argv)
    exit(0)
