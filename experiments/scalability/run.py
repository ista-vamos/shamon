#!/usr/bin/env python3
# coding: utf-8

from tempfile import mkdtemp, mktemp
from shutil import rmtree
from os import chdir, unlink
from measure import *
from sys import argv
from run_common import *
from subprocess import run, TimeoutExpired

NUM="10000000"
TIMEOUT=None #10  # timeout for one experiment

if len(argv) > 1:
    BS = argv[1]
    if len(argv) > 2:
        NUM=argv[2]
else:
    print(f"args: shm-buffer-size [max-number (default: {NUM})]")
    exit(1)

DIR=f"{SHAMONPATH}/experiments/scalability/"
SOURCE_EXE=f"{SHAMONPATH}/experiments/scalability/source"
MONITOR_EXE=f"{SHAMONPATH}/experiments/scalability/monitor"
MONITOR_TXT_IN=f"{SHAMONPATH}/experiments/scalability/monitor.txt.in"

def bs_to_pages(bs):
    # these numbers correspond with 1, 8, 16 and 32 pages
    # 145 1340 2705 5436
    if bs == "145":  return "1"
    if bs == "1340": return "8"
    if bs == "2750": return "16"
    if bs == "5436": return "32"

    raise RuntimeError(f"Invalid buffer size {bs =}")

open_log()
csv = open_csvlog(bs_to_pages(BS), NUM)

WORKINGDIR = mkdtemp(prefix="vamos.", dir="/tmp")
chdir(WORKINGDIR)

lprint(f"-- Working directory is {WORKINGDIR} --")
lprint(f"Sending numbers up to {NUM}th...")

def compile_monitor_txt(buffsize):
    lprint(f"Generating monitor with arbiter bufsize {buffsize} ...")
    outname = MONITOR_TXT_IN[:-3]
    with open(MONITOR_TXT_IN, "r") as infile:
        with open(outname, "w") as outfile:
            for line in infile:
                if "@BUFSIZE" in line:
                    line = line.replace("@BUFSIZE", str(buffsize))
                outfile.write(line)
        with open("/tmp/compile-mon.stdout.txt", "w") as out:
            run(["make", "-C", DIR], check=True, stdout=out)

def run_measurement(source_freq, buffsize):
    source = ParseSource()
    monitor = ParseMonitor()
    shmname = mktemp(prefix="/vamos.ev-")
    duration =\
    measure(f"[SHM {bs_to_pages(BS)} pages] source waits {source_freq} cyc., arbiter buffer has size {buffsize}",
            [Command(SOURCE_EXE, shmname, BS, str(source_freq), NUM).withparser(source)],
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

        for source_freq in (0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200):
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
