#!/usr/bin/python3

from sys import argv
from time import sleep
from subprocess import Popen, PIPE, DEVNULL

REPEAT_NUM=10

DRIOPATH="/opt/dynamorio/"
DRRUN=f"{DRIOPATH}/build/bin64/drrun"
DRIO=[DRRUN, "-root",  f"{DRIOPATH}/build/",
             "-opt_cleancall", "2", "-opt_speed"]
SHAMONPATH="/opt/shamon"
PRIMESPATH=f"{SHAMONPATH}/experiments/primes"

log = open('log.txt', 'w')

def parse_time(err):
    for line in err.splitlines():
        if line.startswith(b'time:'):
            parts = line.split()
            assert len(parts) == 3, parts
            return float(parts[1])

    print("-- ERROR while parsing time (see log.txt)--")
    print("-- ERROR while parsing stderr for time info:", file=log)
    print(err, file=log)
    raise RuntimeError("Did not find time value")

def run_piped(cmd):
    print("[DBG] PRG: ", " | ".join((map(lambda s: " ".join(s), cmd))), file=log)
    procs = []
    max_n = len(cmd) - 1
    for n, c in enumerate(cmd):
        p = Popen(c,
                  stdout=PIPE if n < max_n else DEVNULL,
                  stdin=procs[-1].stdout if n > 0 else PIPE,
                  stderr=PIPE)
        procs.append(p)
    return procs[0], procs

def _measure(cmds, moncmds = (), pipe=False):
    ts = [0] if pipe else [0]*len(cmds)
    for i in range(REPEAT_NUM):
        procs = []
        wait_procs = []
        monprocs = []
        for cmd in cmds:
            if isinstance(cmd, tuple):
                p, tmp = run_piped(cmd)
                wait_procs.extend(tmp)
            else:
                print("[DBG] PRG: ", " ".join(cmd), file=log)
                p = Popen(cmd, stdout=DEVNULL, stderr=PIPE)
            procs.append(p)
        if len(moncmds) > 0:
            sleep(0.15)
        for c in moncmds:
            print("[DBG] MON", " ".join(c), file=log)
            monprocs.append(Popen(c, stdout=DEVNULL, stderr=PIPE))

        for idx, proc in enumerate(procs):
            _, err = proc.communicate()
            ts[idx] += parse_time(err)
            print(f"times: {ts}", file=log)
        for proc in wait_procs:
            ret = proc.wait()
            if ret != 0:
                log.write(f"!! proc had errors\n")
                print(f"\033[0;31m!! PROG had errors (see log.txt)\033[0m")

        for n, mon in enumerate(monprocs):
            _, err = mon.communicate()
            if err:
                log.write(f"!! MON idx {n} has errors\n")
                print(f"\033[0;31m!! MON idx {n} has errors (see log.txt)\033[0m")
                log.write(err.decode('utf-8'))
            mon.kill()
    return list(map(lambda t: str(t/REPEAT_NUM), ts))

def measure(name, cmds, moncmds=(), msg=None):
    print(f"\033[0;34m------- {name} ------\033[0m")
    print(f"------- {name} ------", file=log)
    ts = _measure(cmds, moncmds)
    print("... {0} sec.".format(", ".join(ts)), end=' ' if msg else '\n')
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
measure("'C primes' DynamoRIO (no monitor)",
        [DRIO + ["--", f"{PRIMESPATH}/primes", NUM]])
measure("'C primes' piping ((monitor reads and drops))",
        [([f"{PRIMESPATH}/primes", NUM],
          [f"{SHAMONPATH}/sources/regex",
            "/primes", "prime", "#([0-9]+): ([0-9]+)", "ii"])
        ],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:regex:/primes"]])
measure("'C primes' DynamoRIO ((monitor reads and drops))",
        [DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", f"{PRIMESPATH}/primes", NUM]],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:drregex:/primes1"]])
print('------------------')
measure("'Python primes' alone", [[f"{PRIMESPATH}/primes.py", NUM]])
measure("'Python primes' DynamoRIO (no monitor)",
        [DRIO + ["--", "python3", f"{PRIMESPATH}/primes.py", NUM]])
measure("'Python primes' piping (monitor reads and drops)",
        [(["python3", f"{PRIMESPATH}/primes.py", NUM],
          [f"{SHAMONPATH}/sources/regex",
           "/primes", "prime", "#([0-9]+): ([0-9]+)", "ii"])
        ],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:regex:/primes"]])
measure("'Python primes' DynamoRIO (monitor reads and drops)",
        [DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", "python3", f"{PRIMESPATH}/primes.py", NUM]],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:drregex:/primes1"]])
print('------------------')
measure("'Differential monitor for primes' piping",
        [([f"{PRIMESPATH}/primes", NUM],
          [f"{SHAMONPATH}/sources/regex",
           "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"]),
         (["python3", f"{PRIMESPATH}/primes.py", NUM],
          [f"{SHAMONPATH}/sources/regex",
           "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"])
        ],
        [[f"{SHAMONPATH}/mmtest/monitor",
         "Left:drregex:/primes1", "Right:drregex:/primes2"]],
         msg="First C, second Python")
measure("'Differential monitor for primes' DynamoRIO sources",
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

