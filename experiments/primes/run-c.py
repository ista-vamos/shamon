#!/usr/bin/env python3
# coding: utf-8

from tempfile import mkdtemp
from shutil import rmtree
from os import chdir
from measure import *
from sys import argv
from run_common import *

NUM="10000"
if len(argv) > 1:
    BS = argv[1]
    if len(argv) > 2:
        NUM=argv[2]
else:
    print("args: buffer-size [number of primes]")
    print("buffer-size is the compiled (!) size, it does not set the size.")
    exit(1)

open_log()
open_csvlog(BS, NUM)

WORKINGDIR = mkdtemp(prefix="midmon.", dir="/tmp")
chdir(WORKINGDIR)
lprint(f"-- Working directory is {WORKINGDIR} --")

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

lprint("-- Compiling differential monitor --")
compile_monitor(COMPILESH, PRIMESMONSRC)

###############################################################################

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
close_csvlog()

# chdir("/")
# rmtree(WORKINGDIR)

