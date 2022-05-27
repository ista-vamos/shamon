#!/usr/bin/env python3
# coding: utf-8

from tempfile import mkdtemp
from shutil import rmtree
from os import chdir
from measure import *

SHAMONPATH="/opt/shamon"
DRIOPATH="/opt/dynamorio/"
set_repeat_num(10)
NUM="10000"

WORKINGDIR = mkdtemp(prefix="midmon.", dir="/tmp")
DRRUN=f"{DRIOPATH}/build/bin64/drrun"
DRIO=[DRRUN, "-root",  f"{DRIOPATH}/build/",
             "-opt_cleancall", "2", "-opt_speed"]
PRIMESPATH=f"{SHAMONPATH}/experiments/primes"
PRIMESSRC=f"{SHAMONPATH}/mmtest/mmprimes.c"
COMPILESH=f"{SHAMONPATH}/gen/compile.sh"

open_log()


chdir(WORKINGDIR)
lprint(f"-- Working directory is {WORKINGDIR} --")

lprint("-- Compiling monitor --")
compile_monitor(COMPILESH, PRIMESSRC)

class ParseTime:
    def __init__(self, dbg=False):
        self.times = []
        self.time = 0
        self.dbg = dbg

    def parse(self, out, err):
        if self.dbg:
            for line in out.splitlines():
                lprint(f"[stdout] {line}", color=GRAY, end='')
            for line in err.splitlines():
                lprint(f"[stdout] {line}", color=GRAY, end='')
        for line in err.splitlines():
            if line.startswith(b'time:'):
                parts = line.split()
                assert len(parts) == 3, parts
                f = float(parts[1])
                assert f > 0
                self.times.append(f)
                self.time += f
                return

        log("-- ERROR while parsing stderr for time info:",
            "-- ERROR while parsing time (see log.txt)--")
        log(err)
        raise RuntimeError("Did not find time value")

    def report(self, msg=None):
        assert self.time > 0, f"Invalid time: {self.time}"
        if msg:
            lprint(msg, end=' ', color="\033[0;32m")
        val = self.time / repeat_num()
        lprint(f"Avg CPU time ({repeat_num()} measurements): {val} seconds.",
               color="\033[0;32m")
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
                print((dl, sl, il, pl),(dr, sr, ir, pr), errs)
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
        
    def report(self, msg=None):
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
        return ((dl, sl, il, pl),(dr, sr, ir, pr), errs)


lprint(f"Enumerating primes up to {NUM}th prime...")
lprint(f"Taking average of {repeat_num()} runs...\n")


# In[11]:


c_native = ParseTime()
measure("'C primes' alone",
        [Command(f"{PRIMESPATH}/primes", NUM).withparser(c_native)])
c_native.report()


# In[12]:


c_drio = ParseTime()
measure("'C primes' DynamoRIO (no monitor)",
        [Command(*DRIO, "--", f"{PRIMESPATH}/primes", NUM).withparser(c_drio)])
c_drio.report()


c_pipes = ParseTime()
measure("'C primes' piping (monitor reads and drops)",
        [PipedCommands(Command(f"{PRIMESPATH}/primes", NUM).withparser(c_pipes),
                       Command(f"{SHAMONPATH}/sources/regex", "/primes",
                                "prime", "#([0-9]+): ([0-9]+)", "ii"))],
        [Command(f"{SHAMONPATH}/experiments/monitor-consume",
                  "primes:regex:/primes")])
c_pipes.report()


c_drio_consume = ParseTime()
measure("'C primes' DynamoRIO (monitor reads and drops)",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii",
                 "--", f"{PRIMESPATH}/primes", NUM).withparser(c_drio_consume)],
        [Command(f"{SHAMONPATH}/experiments/monitor-consume",
                  "primes:drregex:/primes1")])
c_drio_consume.report()

python_native = ParseTime()
measure("'Python primes' alone",
        [Command(f"{PRIMESPATH}/primes.py", NUM).withparser(python_native)])
python_native.report()


python_drio = ParseTime()
measure("'Python primes' DynamoRIO (no monitor)",
        [Command(*DRIO, "--", "python3",
                 f"{PRIMESPATH}/primes.py", NUM).withparser(python_drio)])
python_drio.report()


# In[ ]:


python_pipe = ParseTime()
measure("'Python primes' piping (monitor reads and drops)",
        [PipedCommands(Command("python3", f"{PRIMESPATH}/primes.py", NUM).withparser(python_pipe),
                       Command(f"{SHAMONPATH}/sources/regex", "/primes", "prime", "#([0-9]+): ([0-9]+)", "ii"))],
        [Command(f"{SHAMONPATH}/experiments/monitor-consume", "primes:regex:/primes")])
python_pipe.report()


# In[ ]:


python_drio_consume = ParseTime()
measure("'Python primes' DynamoRIO (monitor reads and drops)",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so", 
                 "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii",
                 "--", "python3", f"{PRIMESPATH}/primes.py", NUM).withparser(python_drio_consume)],
        [Command(f"{SHAMONPATH}/experiments/monitor-consume", "primes:drregex:/primes1")])
python_drio_consume.report()


# In[ ]:


dm_pipes_time_c = ParseTime()
dm_pipes_time_py = ParseTime()
dm_pipes_stats = ParseStats()
measure("'Differential monitor C/Py for primes' piping",
        [PipedCommands(Command(f"{PRIMESPATH}/primes", NUM).withparser(dm_pipes_time_c),
                       Command(f"{SHAMONPATH}/sources/regex", "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii")),
         PipedCommands(Command("python3", f"{PRIMESPATH}/primes.py", NUM).withparser(dm_pipes_time_py),
                       Command(f"{SHAMONPATH}/sources/regex", "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"))],
        [Command("./monitor", "Left:drregex:/primes1", "Right:drregex:/primes2").withparser(dm_pipes_stats)])
dm_pipes_time_c.report(msg="C program")
dm_pipes_time_py.report(msg="Python program")
dm_pipes_stats.report()


# In[ ]:


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
        [Command("./monitor", "Left:drregex:/primes1", "Right:drregex:/primes2").withparser(dm_drio_stats)])
dm_drio_time_c.report(msg="C program")
dm_drio_time_py.report(msg="Python program")
dm_drio_stats.report()


# In[ ]:


dm_pipes_time_c1 = ParseTime()
dm_pipes_time_c2 = ParseTime()
dm_pipes_stats2 = ParseStats()
measure("'Differential monitor C/C for primes' piping",
        [PipedCommands(Command(f"{PRIMESPATH}/primes", NUM).withparser(dm_pipes_time_c1),
                       Command(f"{SHAMONPATH}/sources/regex", "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii")),
         PipedCommands(Command(f"{PRIMESPATH}/primes", NUM).withparser(dm_pipes_time_c2),
                       Command(f"{SHAMONPATH}/sources/regex", "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"))],
        [Command("./monitor", "Left:drregex:/primes1", "Right:drregex:/primes2").withparser(dm_pipes_stats2)])
dm_pipes_time_c1.report(msg="C program (1)")
dm_pipes_time_c2.report(msg="C program (2)")
dm_pipes_stats2.report()


# In[ ]:


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
        [Command("./monitor", "Left:drregex:/primes1", "Right:drregex:/primes2").withparser(dm_drio_stats2)])
dm_drio_time_c1.report(msg="C program")
dm_drio_time_c2.report(msg="Python program")
dm_drio_stats2.report()


# In[ ]:


dm_pipes_bad_time_c1 = ParseTime()
dm_pipes_bad_time_c2 = ParseTime()
dm_pipes_bad_stats2 = ParseStats()
measure("'Differential monitor C/C for primes' piping, 10% mistakes",
        [PipedCommands(Command(f"{PRIMESPATH}/primes", NUM).withparser(dm_pipes_bad_time_c1),
                       Command(f"{SHAMONPATH}/sources/regex", "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii")),
         PipedCommands(Command(f"{PRIMESPATH}/primes-bad", NUM, str(int(NUM)/10)).withparser(dm_pipes_bad_time_c2),
                       Command(f"{SHAMONPATH}/sources/regex", "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"))],
        [Command("./monitor", "Left:drregex:/primes1", "Right:drregex:/primes2").withparser(dm_pipes_bad_stats2)])
dm_pipes_bad_time_c1.report(msg="C program (1)")
dm_pipes_bad_time_c2.report(msg="C program (2)")
dm_pipes_bad_stats2.report()


# In[ ]:


log(f"-- Removing working directory {WORKINGDIR} --")
close_log()
# chdir("/")
# rmtree(WORKINGDIR)

