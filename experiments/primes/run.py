#!/usr/bin/env python3
# coding: utf-8

from tempfile import mkdtemp
from shutil import rmtree
from os import chdir
from measure import *
from sys import argv
from csv import writer as csvwriter

SHAMONPATH="/opt/shamon"
DRIOPATH="/opt/dynamorio/"
set_repeat_num(20)
NUM="10000"
if len(argv) > 1:
    BS = argv[1]
    if len(argv) > 2:
        NUM=argv[2]
else:
    print("args: buffer-size [number of primes]")
    print("buffer-size is the compiled (!) size, it does not set the size.")
    exit(1)

WORKINGDIR = mkdtemp(prefix="midmon.", dir="/tmp")
DRRUN=f"{DRIOPATH}/build/bin64/drrun"
DRIO=[DRRUN, "-root",  f"{DRIOPATH}/build/",
             "-opt_cleancall", "2", "-opt_speed"]
PRIMESPATH=f"{SHAMONPATH}/experiments/primes"
PRIMESMONSRC=f"{SHAMONPATH}/mmtest/mmprimes.c"
EMPTYMONSRC=f"{SHAMONPATH}/mmtest/mmempty.c"
COMPILESH=f"{SHAMONPATH}/gen/compile.sh"

csv_name = f"times-{BS}-{NUM}.csv"
csvlogf = open(csv_name, 'a')
csvlog = csvwriter(csvlogf)

open_log()

chdir(WORKINGDIR)
lprint(f"-- Working directory is {WORKINGDIR} --")

class ParseTime:
    def __init__(self, dbg=False, withwaitstats=True):
        self.waiting = []
        self.times = []
        self.time = 0
        self.waited = 0
        self.dbg = dbg
        self.withwaitstats = withwaitstats

    def parse(self, out, err):
        if self.dbg:
            if out:
               for line in out.splitlines():
                   lprint(f"[stdout] {line}", color=GRAY, end='')
            if err:
                for line in err.splitlines():
                    lprint(f"[stderr] {line}", color=GRAY, end='')

        foundwaited = False
        foundtime = False
        for line in err.splitlines():
            if line.startswith(b'time:'):
                parts = line.split()
                assert len(parts) == 3, parts
                f = float(parts[1])
                assert f > 0
                self.times.append(f)
                self.time += f
                foundtime = True
                #return
            if b'busy waited' in line:
                parts = line.split()
                assert len(parts) == 10, parts
                c = int(parts[8])
                self.waited += c
                self.waiting.append(c)
                foundwaited = True

        if not foundtime or\
            (self.withwaitstats and not foundwaited):
            log("-- ERROR while parsing stderr for time info:",
                "-- ERROR while parsing time (see log.txt)--")
            log(err)
            raise RuntimeError("Did not find right values")

    def report(self, key, msg=None):
        assert self.time > 0, f"Invalid time: {self.time}"
        if msg:
            lprint(msg, end=' ', color="\033[0;32m")
        val = self.time / repeat_num()
        lprint(f"Avg CPU time ({repeat_num()} measurements): {val} seconds.",
               color="\033[0;32m")
        for t, c in zip(self.times, self.waiting):
            csvlog.writerow([key, t, c])
        return val

class ParseStats:
    def __init__(self):
        self.stats = []
        self.dl, self.dr = None, None
        self.sl, self.sr = None, None
        self.il, self.ir = None, None
        self.pl, self.pr = None, None
        self.errs = None

    def parse(self, out, err):
        errs = 0
        for line in out.splitlines():
            if line.startswith(b'ERROR'):
                errs += 1
            if line.startswith(b'Done!'):
                parts = line.split()
                assert len(parts) == 25, parts
                dl, dr = int(parts[2]), int(parts[5])
                sl, sr = int(parts[8]), int(parts[11])
                il, ir = int(parts[14]), int(parts[17])
                pl, pr = int(parts[20]), int(parts[23])
                if dl + sl + il + pl == 0 or dl + sl + il + pl != dr + sr + ir + pr:
                    log(out)
                    lprint(f"left: {(dl, sl, il, pl)}, right: {(dr, sr, ir, pr)}, errs: {errs}")
                    lprint("Did not find right values", color=RED)
                #print((dl, sl, il, pl),(dr, sr, ir, pr), errs)
                self.stats.append(((dl, sl, il, pl),(dr, sr, ir, pr), errs))
                self.dl, self.dr = dl, dr
                self.sl, self.sr = sl, sr
                self.il, self.ir = il, ir
                self.pl, self.pr = pl, pr
                self.errs = errs
                return

        lprint("-- ERROR while parsing monitor output (see log.txt)--")
        log(out)
        raise RuntimeError("Did not find right values")

    def report(self, key, msg=None):
        assert self.stats
        if msg:
            lprint(msg, end=' ', color="\033[0;32m")
        # FIXME: average is not a good statistics here
        dl = sum(self.stats[i][0][0] for i in range(len(self.stats)))
        dr = sum(self.stats[i][1][0] for i in range(len(self.stats)))
        sl = sum(self.stats[i][0][1] for i in range(len(self.stats)))
        sr = sum(self.stats[i][1][1] for i in range(len(self.stats)))
        il = sum(self.stats[i][0][2] for i in range(len(self.stats)))
        ir = sum(self.stats[i][1][2] for i in range(len(self.stats)))
        pl = sum(self.stats[i][0][3] for i in range(len(self.stats)))
        pr = sum(self.stats[i][1][3] for i in range(len(self.stats)))
        errs = sum(self.stats[i][2] for i in range(len(self.stats)))
        N = repeat_num()
        lprint(\
f"""
For {NUM} incoming events, average of {N} measurements:

Left dropped: {dl / N}
Right dropped: {dr / N}
Left skipped: {sl / N}
Right skipped: {sr / N}
Left ignored: {il / N}
Right ignored: {ir / N}
Left processed: {pl / N}
Right processed: {pr / N}
Detected errors: {errs/ N}""",
               color=GREEN)

        for Sl, Sr, E in self.stats:
            csvlog.writerow([key, *Sl, *Sr, E])
        return ((dl, sl, il, pl),(dr, sr, ir, pr), errs)

#####################################################################

lprint(f"Enumerating primes up to {NUM}th prime...")
lprint(f"Taking average of {repeat_num()} runs...\n")

lprint("-- Compiling empty monitor --")
compile_monitor(COMPILESH, EMPTYMONSRC)

###############################################################################

c_native = ParseTime(withwaitstats=False)
measure("'C primes' native",
        [Command(f"{PRIMESPATH}/primes", NUM).withparser(c_native)])
c_native.report('c-native')

c_drio = ParseTime(withwaitstats=False)
measure("'C primes' DynamoRIO (no monitor)",
        [Command(*DRIO, "--", f"{PRIMESPATH}/primes", NUM).withparser(c_drio)])
c_drio.report('c-drio')

dm_drio_consume_time_c1 = ParseTime()
dm_drio_consume_time_c2 = ParseTime()
measure("'Empty monitor C/C for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes", NUM).withparser(dm_drio_consume_time_c1),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes", NUM).withparser(dm_drio_consume_time_c2)],
        [Command("./monitor", "Left:drregex:/primes1", "Right:drregex:/primes2",
                 stdout=PIPE)])
dm_drio_consume_time_c1.report('c-c-empty', msg="C (1) program")
dm_drio_consume_time_c2.report('c-c-empty', msg="C (2) program")

###############################################################################

python_native = ParseTime(withwaitstats=False)
measure("'Python primes' native",
        [Command(f"{PRIMESPATH}/primes.py", NUM).withparser(python_native)])
python_native.report('py-native')


python_drio = ParseTime(withwaitstats=False)
measure("'Python primes' DynamoRIO (no monitor)",
        [Command(*DRIO, "--", "python3",
                 f"{PRIMESPATH}/primes.py", NUM).withparser(python_drio)])
python_drio.report('py-drio')

dm_drio_consume_time_py1 = ParseTime()
dm_drio_consume_time_py2 = ParseTime()
measure("'Empty monitor Py/Py for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "python3", f"{PRIMESPATH}/primes.py", NUM).withparser(dm_drio_consume_time_py1),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "python3", f"{PRIMESPATH}/primes.py", NUM).withparser(dm_drio_consume_time_py2)],
        [Command("./monitor", "Left:drregex:/primes1",
                 "Right:drregex:/primes2", stdout=PIPE)])
dm_drio_consume_time_py1.report('py-py-empty', msg="Python program (1)")
dm_drio_consume_time_py2.report('py-py-empty', msg="Python program (2)")

###############################################################################

lprint("-- Compiling differential monitor --")
compile_monitor(COMPILESH, PRIMESMONSRC)

###############################################################################

dm_drio_time_c = ParseTime()
dm_drio_time_py = ParseTime()
dm_drio_stats = ParseStats()
measure("'Differential monitor C/Py for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes", NUM).withparser(dm_drio_time_c),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "python3", f"{PRIMESPATH}/primes.py", NUM).withparser(dm_drio_time_py)],
        [Command("./monitor", "Left:drregex:/primes1",
                 "Right:drregex:/primes2", stdout=PIPE).withparser(dm_drio_stats)])
dm_drio_time_c.report('c-py-dm-c', msg="C program")
dm_drio_time_py.report('c-py-dm-py', msg="Python program")
dm_drio_stats.report('c-py-dm-stats')

dm_drio_time_c1 = ParseTime()
dm_drio_time_c2 = ParseTime()
dm_drio_stats2 = ParseStats()
measure("'Differential monitor C/C for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes", NUM).withparser(dm_drio_time_c1),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes", NUM).withparser(dm_drio_time_c2)],
        [Command("./monitor", "Left:drregex:/primes1", "Right:drregex:/primes2",
                 stdout=PIPE).withparser(dm_drio_stats2)])
dm_drio_time_c1.report('c-c-dm', msg="C (1) program")
dm_drio_time_c2.report('c-c-dm', msg="C (2) program")
dm_drio_stats2.report('c-c-dm-stats')

dm_drio_time_py1 = ParseTime()
dm_drio_time_py2 = ParseTime()
dm_drio_stats_pp = ParseStats()
measure("'Differential monitor Py/Py for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "python3", f"{PRIMESPATH}/primes.py", NUM).withparser(dm_drio_time_py1),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "python3", f"{PRIMESPATH}/primes.py", NUM).withparser(dm_drio_time_py2)],
        [Command("./monitor", "Left:drregex:/primes1",
                 "Right:drregex:/primes2", stdout=PIPE).withparser(dm_drio_stats_pp)])
dm_drio_time_py1.report('py-py-dm', msg="Python program")
dm_drio_time_py2.report('py-py-dm', msg="Python program")
dm_drio_stats_pp.report('py-py-dm-stats')

dm_drio_bad_time_c1 = ParseTime()
dm_drio_bad_time_c2 = ParseTime()
dm_drio_bad_stats2 = ParseStats()
measure("'Differential monitor C/C for primes' DynamoRIO sources, 10% errors",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes", NUM).withparser(dm_drio_bad_time_c1),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes-bad", NUM, str(int(NUM)/10)).withparser(dm_drio_bad_time_c2)],
        [Command("./monitor", "Left:drregex:/primes1",
                 "Right:drregex:/primes2", stdout=PIPE).withparser(dm_drio_bad_stats2)])
dm_drio_bad_time_c1.report('c-c-dm-errs-good', msg="C (1) program (good)")
dm_drio_bad_time_c2.report('c-c-dm-errs-bad', msg="C (2) program (bad)")
dm_drio_bad_stats2.report('c-c-dm-errs-stats')


log(f"-- Removing working directory {WORKINGDIR} --")
close_log()
csvlogf.close()

# chdir("/")
# rmtree(WORKINGDIR)

