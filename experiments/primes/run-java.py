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

java_native = ParseTime(withwaitstats=False)
measure("'Java primes' native",
        [Command("java", "-cp", PRIMESPATH, "primes", NUM).withparser(java_native)])
java_native.report('java-native')


java_drio = ParseTime(withwaitstats=False)
measure("'Java primes' DynamoRIO (no monitor)",
        [Command(*DRIO, "--", "java", "-cp", PRIMESPATH, "primes", NUM).withparser(java_drio)])
java_drio.report('java-drio')

###############################################################################

# create temporary names for SHM files
primes1 = rand_shm_name('primes1')
primes2 = rand_shm_name('primes2')

###############################################################################

dm_drio_consume_time_java1 = ParseTime()
dm_drio_consume_time_java2 = ParseTime()
measure("'Empty monitor Java/Java for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 primes1, "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "java", "-cp", PRIMESPATH, "primes", NUM).withparser(dm_drio_consume_time_java1),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 primes2, "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "java", "-cp", PRIMESPATH, "primes", NUM).withparser(dm_drio_consume_time_java2)],
        [Command("./monitor", f"Left:drregex:{primes1}",
                 f"Right:drregex:{primes2}", stdout=PIPE)])
dm_drio_consume_time_java1.report('java-java-empty', msg="Java program (1)")
dm_drio_consume_time_java2.report('java-java-empty', msg="Java program (2)")

###############################################################################

lprint("-- Compiling differential monitor --")
compile_monitor(COMPILESH, PRIMESMONSRC, ABS)

dm_drio_time_java1 = ParseTime()
dm_drio_time_java2 = ParseTime()
dm_drio_stats_pp = ParseStats()
measure("'Differential monitor Java/Java for primes' DynamoRIO sources",
        [Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 primes1, "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "java", "-cp", PRIMESPATH, "primes", NUM).withparser(dm_drio_time_java1),
         Command(*DRIO, "-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
                 primes2, "prime", "#([0-9]+): ([0-9]+)", "ii", "--",
                 "java", "-cp", PRIMESPATH, "primes", NUM).withparser(dm_drio_time_java2)],
        [Command("./monitor", f"Left:drregex:{primes1}",
                 f"Right:drregex:{primes2}", stdout=PIPE).withparser(dm_drio_stats_pp)])
dm_drio_time_java1.report('java-java-dm', msg="Java program")
dm_drio_time_java2.report('java-java-dm', msg="Java program")
dm_drio_stats_pp.report('java-java-dm-stats')

log(f"-- Removing working directory {WORKINGDIR} --")
close_log()
close_csvlog()

chdir("/")
rmtree(WORKINGDIR)

