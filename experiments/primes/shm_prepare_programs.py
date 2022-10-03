import os
from subprocess import run, TimeoutExpired

# generate shamon programs with different buffer size
for buffsize in [4, 16, 128, 1024]:
    file = open("./primes.txt.in", "r")
    outfile = open(f"./programs/primes_{buffsize}.txt", "w")
    for line in file:
        if "@BUFSIZE" in line:
            line = line.replace("@BUFSIZE", str(buffsize))
        outfile.write(line)
    outfile.close()
    file.close()

CURRENT_PATH = os.getcwd()
assert("/".join(CURRENT_PATH.split("/")[-3:]) == 'shamon/experiments/primes') # assert we are currently on shamon/experiments/primes
COMPILER_PATH = f"{CURRENT_PATH}/../../compiler/main.py"

# compile shamon programs into c programs
for buffsize in [4, 16, 128, 1024]:
    run(["python3", COMPILER_PATH, f"{CURRENT_PATH}/programs/primes_{buffsize}.txt", f"{CURRENT_PATH}/programs/monitor_{buffsize}.c"], check=True)


# generate oject file of intmap
run(["g++", "-c", f"{CURRENT_PATH}/../../compiler/cfiles/intmap.cpp" ], check=True)

COMPILE_SCRIPT= f"{CURRENT_PATH}/../../gen/compile_primes6.sh"
for buffsize in [4, 16, 128, 1024]:
    # compile c files
    run(["bash", COMPILE_SCRIPT, f"{CURRENT_PATH}/programs/monitor_{buffsize}.c" ], check=True)

    # move exec to /primes/programs
    run(["mv", "monitor", f"{CURRENT_PATH}/programs/monitor{buffsize}" ], check=True)


# compile empty monitor

COMPILE_SCRIPT= f"{CURRENT_PATH}/../../gen/compile.sh"
for buffsize in [4, 16, 128, 1024]:
    # compile c files
    run(["bash", COMPILE_SCRIPT, f"{CURRENT_PATH}/../../mmtest/mmempty.c", f"{buffsize}"], check=True)

    # move exec to /primes/programs
    run(["mv", "monitor", f"{CURRENT_PATH}/programs/empty_monitor{buffsize}" ], check=True)


