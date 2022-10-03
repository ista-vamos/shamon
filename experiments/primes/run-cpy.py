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

EMPTY_MONITOR_PATH = f"{PRIMESPATH}/programs/empty_monitor{ABS}"
MONITOR_PATH = f"{PRIMESPATH}/programs/monitor{ABS}"

open_log()
open_csvlog(BS, ABS, NUM)

WORKINGDIR = mkdtemp(prefix="midmon.", dir="/tmp")
chdir(WORKINGDIR)
lprint(f"-- Working directory is {WORKINGDIR} --")

lprint(f"Enumerating primes up to {NUM}th prime...")
lprint(f"Taking average of {repeat_num()} runs...\n")

lprint("--  Using empty monitor: {EMPTY_MONITOR_PATH} --")

###############################################################################

# create temporary names for SHM files
primes1 = rand_shm_name('primes1')
primes2 = rand_shm_name('primes2')

###############################################################################

dm_drio_time_c = ParseTime()
dm_drio_time_py = ParseTime()
measure("'Empty monitor C/Py for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 primes1, "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes", NUM).withparser(dm_drio_time_c),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 primes2, "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "python3", f"{PRIMESPATH}/primes.py", NUM).withparser(dm_drio_time_py)],
        [Command(EMPTY_MONITOR_PATH, f"Left:drregex:{primes1}", f"Right:drregex:{primes2}",
                 stdout=PIPE)])
dm_drio_time_c.report('c-py-empty-c', msg="C program")
dm_drio_time_py.report('c-py-empty-py', msg="Python program")


###############################################################################

lprint(f"-- Using differential monitor: {MONITOR_PATH} --")

###############################################################################

dm_drio_time_c = ParseTime()
dm_drio_time_py = ParseTime()
dm_drio_stats = ParseStats()
measure("'Differential monitor C/Py for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 primes1, "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 f"{PRIMESPATH}/primes", NUM).withparser(dm_drio_time_c),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 primes2, "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "python3", f"{PRIMESPATH}/primes.py", NUM).withparser(dm_drio_time_py)],
        [Command(MONITOR_PATH, f"P_0:drregex:{primes1}",
                 f"P_1:drregex:{primes2}", stdout=PIPE).withparser(dm_drio_stats)])
dm_drio_time_c.report('c-py-dm-c', msg="C program")
dm_drio_time_py.report('c-py-dm-py', msg="Python program")
dm_drio_stats.report('c-py-dm-stats')
assert dm_drio_stats.errs == 0, "Had errors in non-error traces"


log(f"-- Removing working directory {WORKINGDIR} --")
close_log()
close_csvlog()

chdir("/")
rmtree(WORKINGDIR)

