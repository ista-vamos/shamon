#!/usr/bin/env python3
# coding: utf-8

from tempfile import mkdtemp, mktemp
from shutil import rmtree
from os import chdir, unlink
from measure import *
from sys import argv
from run_common import *
from subprocess import run, TimeoutExpired

NUM="100000"
TIMEOUT=None #10  # timeout for one experiment

if len(argv) > 1:
    BS = argv[1]
    if len(argv) > 2:
        NUM=argv[2]
else:
    print(f"args: shm-buffer-size [max-number (default: {NUM})]")
    print("shm-buffer-size is the compiled (!) size, it does not set the size.")
    exit(1)

DIR=f"{SHAMONPATH}/experiments/scalability/"
SOURCE_EXE=f"{SHAMONPATH}/experiments/scalability/source"
MONITOR_EXE=f"{SHAMONPATH}/experiments/scalability/monitor"
MONITOR_TXT_IN=f"{SHAMONPATH}/experiments/scalability/monitor.txt.in"

open_log()
csv = open_csvlog(BS, NUM)

WORKINGDIR = mkdtemp(prefix="vamos.", dir="/tmp")
chdir(WORKINGDIR)

lprint(f"-- Working directory is {WORKINGDIR} --")
lprint(f"Sending numbers up to {NUM}th...")

def compile_monitor_txt(buffsize):
    lprint(f"Generating monitor with arbiter bufsize {buffsize}")
    outname = MONITOR_TXT_IN[:-3]
    with open(MONITOR_TXT_IN, "r") as infile:
        with open(outname, "w") as outfile:
            for line in infile:
                if "@BUFSIZE" in line:
                    line = line.replace("@BUFSIZE", str(buffsize))
                outfile.write(line)
    run(["make", "-C", DIR], check=True)

def run_measurement(source_freq, buffsize):
    source = ParseSource()
    monitor = ParseMonitor()
    shmname = mktemp(prefix="/vamos.ev-")
    duration =\
    measure(f"Source waits {source_freq} cycles, arbiter buffer has size {buffsize}",
            [Command(SOURCE_EXE, shmname, str(source_freq), NUM).withparser(source)],
            [Command(MONITOR_EXE, f"Src:generic:{shmname}",
                     stdout=PIPE).withparser(monitor)],
            timeout=TIMEOUT)
    print("duration    src-wait     mon-proc     mon-drop     mon-drop-evs"),
    print(f"{duration :<12.5f}"
          f"{source.waiting[0] :<12}",
          f"{monitor.processed :<12}",
          f"{monitor.dropped :<12}",
          f"{monitor.dropped_times :<12}")
    wrong_values = None
    if source.sent != int(NUM):
        wrong_values = "source"
    if monitor.processed + monitor.dropped != int(NUM):
        wrong_values = "monitor"

    if wrong_values:
        for line in source.lines:
            lprint(f"SOURCE: {line}")
        for line in monitor.lines:
            lprint(f"MONITOR: {line}")
        raise RuntimeError(f"Wrong values found, is {wrong_values} buggy?")

    csv.writerow([source_freq, buffsize,
                  source.waiting[0], monitor.processed,
                  monitor.dropped, monitor.dropped_times, duration])

retval = 0

try:
    for buffsize in (4, 8, 16, 128, 1024, 2048):
        compile_monitor_txt(buffsize)

        for source_freq in (0, 10, 100, 1000, 10000):
            try:
                run_measurement(source_freq, buffsize)
            except TimeoutExpired:
                lprint("TIMEOUT!", color="red")
                retval = -1
except Exception as e:
    lprint(str(e))
    retval = -1
finally:
    close_log()
    close_csvlog()

    chdir("/")
    rmtree(WORKINGDIR)

    exit(retval)
