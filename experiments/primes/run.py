#!/usr/bin/python3

from sys import argv
from time import sleep
from subprocess import Popen, PIPE, DEVNULL

REPEAT_NUM=10

DRIOPATH="/opt/dynamorio/"
DRRUN=f"{DRIOPATH}/build/bin64/drrun"
DRIO=[DRRUN, "-root",  f"{DRIOPATH}/build/"]
SHAMONPATH="/opt/shamon"
PRIMESPATH=f"{SHAMONPATH}/experiments/primes"

def parse_time(err):
    for line in err.splitlines():
        if b'time:' in line:
            parts = line.split()
            assert len(parts) == 3
            return float(parts[1])

    print("-- ERROR --")
    print(err)
    raise RuntimeError("Did not find time value")

def _measure(cmd, *moncmds):
    monprocs = []
    t = 0
    for i in range(REPEAT_NUM):
        # RUN PROCESS
        proc = Popen(cmd, stdout=DEVNULL, stderr=PIPE)
        sleep(0.2)
        for c in moncmds:
            #print("[DBG] MON", " ".join(c))
            mon = Popen(c, stdout=DEVNULL, stderr=PIPE)
            monprocs.append(mon)
        _, err = proc.communicate()
        t += parse_time(err)

        for mon in monprocs:
            ret = mon.wait()
            if ret != 0:
                print("Monitor return non-0 return value")
            mon.kill()
    return t/REPEAT_NUM

def measure(name, cmd, *cmds):
    print("[DBG]: ", " ".join(cmd))
    print(f"Running {name}", end='')
    t = _measure(cmd, *cmds)
    print(f"... {t} sec.")
    return t

if len(argv) > 1:
    NUM=argv[1]
else:
    NUM="1000"

print(f"Enumerating primes up to {NUM}th prime...")
print(f"Taking average of {REPEAT_NUM} runs...\n")

measure("'primes' alone", [f"{PRIMESPATH}/primes", NUM])
measure("'primes' DynamoRIO (no monitor)",
        DRIO + ["--", f"{PRIMESPATH}/primes", NUM])
measure("'primes' DynamoRIO (only instrumentation)",
        DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", f"{PRIMESPATH}/primes", NUM],
        [f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:drregex:/primes1"])
measure("'primes' DynamoRIO (with monitor)",
        DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", f"{PRIMESPATH}/primes", NUM],
        DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", f"{PRIMESPATH}/primes.py", NUM],
        [f"{SHAMONPATH}/mmtest/monitor",
         "Left:drregex:/primes1", "Right:drregex:/primes1"])


# $DRRIO -c $SHAMONPATH/sources/drregex/libdrregex.so -- $PRIMESPATH/primes $NUM >primes1out.txt &
# PRIMES1_PID=$!
# echo "Press key to continue..."; read N
# 
# $DRIOPATH/build/bin64/drrun -root $DRIOPATH/build/ -c $SHAMONPATH/sources/drsyscalls/libsyscalls.so -- python3 $PRIMESPATH/primes.py $NUM >primes2out.txt &
# PRIMES2_PID=$!
# echo "Press key to continue..."; read N
# 
# $SHAMONPATH/sources/regexd $PRIMES1_PID /primes1 prime "\s*\#([0-9]+):\s+([0-9]+)\s*" ii &
# echo "Press key to continue..."; read N
# 
# $SHAMONPATH/sources/regexd $PRIMES2_PID /primes2 prime "\s*\#([0-9]+):\s+([0-9]+)\s*" ii &
# echo "Press key to continue..."; read N
# 
# $SHAMONPATH/mmtest/monitor Left:regex:/primes1 Right:regex:/primes2 > log.txt
# 
# kill -9 $PRIMES1_PID $PRIMES2_PID
