#!/usr/bin/env python3
# coding: utf-8

from tempfile import mkdtemp
from shutil import rmtree
from os import chdir
from measure import *
from sys import argv
from run_common import *

NUM="10000"
if len(argv) > 2:
    BS = argv[1]
    ABS = argv[2]
    if len(argv) > 3:
        NUM=argv[3]
else:
    print("args: shm-buffer-size arbiter-buffer-size [number of primes]")
    print("shm-buffer-size is the compiled (!) size, it does not set the size.")
    print("arbiter-buffer-size is will set the size of the arbiter buffer in the monitor.")
    exit(1)



open_log()
open_csvlog(BS, NUM)

WORKINGDIR = mkdtemp(prefix="midmon.", dir="/tmp")
chdir(WORKINGDIR)
lprint(f"-- Working directory is {WORKINGDIR} --")

lprint(f"Enumerating primes up to {NUM}th prime...")
lprint(f"Taking average of {repeat_num()} runs...\n")

lprint("-- Compiling empty monitor --")
compile_monitor(COMPILESH, EMPTYMONSRC, ABS)

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
compile_monitor(COMPILESH, PRIMESMONSRC, ABS)

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

log(f"-- Removing working directory {WORKINGDIR} --")
close_log()
close_csvlog()

# chdir("/")
# rmtree(WORKINGDIR)

