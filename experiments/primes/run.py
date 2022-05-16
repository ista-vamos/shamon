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
        if line.startswith(b'time:'):
            parts = line.split()
            assert len(parts) == 3, parts
            return float(parts[1])

    print("-- ERROR --")
    print(err)
    raise RuntimeError("Did not find time value")

log = open('log.txt', 'w')

def _measure(cmds, moncmds = ()):
    ts = [0]*len(cmds)
    for i in range(REPEAT_NUM):
        procs = []
        monprocs = []
        for cmd in cmds:
            print("[DBG] PRG: ", " ".join(cmd), file=log)
            procs.append(Popen(cmd, stdout=DEVNULL, stderr=PIPE))
        if len(moncmds) > 0:
            sleep(0.15)
        for c in moncmds:
            print("[DBG] MON", " ".join(c), file=log)
            monprocs.append(Popen(c, stdout=DEVNULL, stderr=PIPE))

        for idx, proc in enumerate(procs):
            _, err = proc.communicate()
            ts[idx] += parse_time(err)
            print(f"times: {ts}", file=log)

        for n, mon in enumerate(monprocs):
            _, err = mon.communicate()
            if err:
                log.write(f"!! MON idx {n} has errors\n")
                log.write(err)
            mon.kill()
    return list(map(lambda t: str(t/REPEAT_NUM), ts))

def measure(name, cmds, moncmds=(), msg=None):
    print(f"------- {name} ------", end='')
    print(f"------- {name} ------", file=log)
    ts = _measure(cmds, moncmds)
    print("... {0} sec.".format(", ".join(ts)), end='' if msg else '\n')
    print("... {0} sec.".format(", ".join(ts)), end='' if msg else '\n',
          file=log)
    if msg:
        print(msg)
        print(msg, file=log)
    return ts

if len(argv) > 1:
    NUM=argv[1]
else:
    NUM="10000"

print(f"Enumerating primes up to {NUM}th prime...")
print(f"Taking average of {REPEAT_NUM} runs...\n")

measure("'C primes' alone", [[f"{PRIMESPATH}/primes", NUM]])
measure("'Python primes' alone", [[f"{PRIMESPATH}/primes.py", NUM]])
measure("'C primes' DynamoRIO (no monitor)",
        [DRIO + ["--", f"{PRIMESPATH}/primes", NUM]])
measure("'Python primes' DynamoRIO (no monitor)",
        [DRIO + ["--", "python3", f"{PRIMESPATH}/primes.py", NUM]])
measure("'C primes' DynamoRIO (only instrumentation)",
        [DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", f"{PRIMESPATH}/primes", NUM]],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:drregex:/primes1"]])
measure("'Python primes' DynamoRIO (only instrumentation)",
        [DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", "python3", f"{PRIMESPATH}/primes.py", NUM]],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:drregex:/primes1"]])

measure("'Differential primes' DynamoRIO (with monitor)",
        [DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", f"{PRIMESPATH}/primes", NUM],
        DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", "python3", f"{PRIMESPATH}/primes.py", NUM]],
        [[f"{SHAMONPATH}/mmtest/monitor",
         "Left:drregex:/primes1", "Right:drregex:/primes2"]],
         msg="First C, second Python")

