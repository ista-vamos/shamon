#!/usr/bin/env python3
# coding: utf-8

from tempfile import mkdtemp
from shutil import rmtree
from os import chdir
from measure import *
from sys import argv
from run_common import *
from time import clock_gettime, CLOCK_MONOTONIC

NUM="10000000"
if len(argv) > 1:
    BS = argv[1]
    if len(argv) > 2:
        NUM=argv[2]
else:
    print(f"args: buffer-size [max-number (default: {NUM})]")
    print("buffer-size is the compiled (!) size, it does not set the size.")
    exit(1)

SOURCE_EXE=f"{SHAMONPATH}/experiments/scalability/source"
MONITOR_EXE=f"{SHAMONPATH}/experiments/scalability/monitor"

open_log()
csv = open_csvlog(BS, NUM)

WORKINGDIR = mkdtemp(prefix="midmon.", dir="/tmp")
chdir(WORKINGDIR)
lprint(f"-- Working directory is {WORKINGDIR} --")

lprint(f"Sending numbers up to {NUM}th...")
lprint(f"Taking average of {repeat_num()} runs...\n")

def run_measurement(source_freq, monitor_freq, buffsize, max_consume):
    source = ParseSource()
    monitor = ParseMonitor()
    start = clock_gettime(CLOCK_MONOTONIC)
    measure(f"Source waits {source_freq} cycles, monitor waits {monitor_freq}, "
            f"buffer has size {buffsize} and can consume at most {max_consume} "
             "events at once",
            [Command(SOURCE_EXE, "/ev", str(source_freq), NUM).withparser(source)],
            [Command(MONITOR_EXE, "stream:generic:/ev",
                     str(buffsize), str(monitor_freq), str(max_consume),
                     stdout=PIPE).withparser(monitor)])
    end = clock_gettime(CLOCK_MONOTONIC)
    print("duration    src-wait     mon-proc     mon-drop     mon-drop-evs"),
    print(f"{end-start :<12.5f}"
          f"{source.waiting[0] :<12}",
          f"{monitor.processed :<12}",
          f"{monitor.dropped :<12}",
          f"{monitor.dropped_times :<12}")
    csv.writerow([source_freq, monitor_freq, buffsize, max_consume,
                  source.waiting[0], monitor.processed,
                  monitor.dropped, monitor.dropped_times, end-start])

for source_freq in (0, 100, 500, 1000, 5000, 10000):
    for monitor_freq in (0, 100, 500, 1000, 5000, 10000):
        for buffsize in (10, 100, 1000, 10000):
            for max_consume in (1, 2, 5, 10, 50, 100, 1000, 1<<64-1):
                run_measurement(source_freq, monitor_freq, buffsize, max_consume)
close_log()
close_csvlog()

chdir("/")
rmtree(WORKINGDIR)

