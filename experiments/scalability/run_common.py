from measure import *
from csv import writer as csvwriter
from sys import argv
from os.path import dirname, abspath, join as pathjoin

SHAMONPATH=abspath(pathjoin(dirname(argv[0]), "../..")) # "/opt/shamon"

csvlog = None
csvlogf = None

def open_csvlog(BS, NUM, SRCNUM=1):
    csv_name = f"numbers-{BS}-{NUM}-{SRCNUM}.csv"

    global csvlogf
    csvlogf = open(csv_name, 'a')

    global csvlog
    csvlog = csvwriter(csvlogf)
    return csvlog

def close_csvlog():
    csvlogf.close()

class ParseSource:
    def __init__(self, dbg=False):
        self.waiting = []
        self.waited = 0
        self.sent = 0
        self.dbg = dbg
        self.lines = []

    def parse(self, out, err):
        if self.dbg:
            if out:
               for line in out.splitlines():
                   lprint(f"[stdout] {line}", color=GRAY, end='')
            if err:
                for line in err.splitlines():
                    lprint(f"[stderr] {line}", color=GRAY, end='')

        foundwaited = False
        for line in err.splitlines():
            if b'busy waited' in line:
                self.lines.append(line)
                parts = line.split()
                assert len(parts) == 10, parts
                self.sent = int(parts[2])
                c = int(parts[8])
                self.waited += c
                self.waiting.append(c)
                foundwaited = True

        if not foundwaited:
            log("-- ERROR while parsing stderr for time info:",
                "-- ERROR while parsing time (see log.txt)--")
            log(err)
            raise RuntimeError("Did not find right values")


class ParseMonitor:
    def __init__(self):
        self.processed = None
        self.dropped = None
        self.dropped_times = None
        self.lines = []

    def parse(self, out, err):
        for line in out.splitlines():
            if line.startswith(b'Processed '):
                self.lines.append(line)
                parts = line.split()
                assert len(parts) == 9, parts
                self.processed = int(parts[1])
                self.dropped = int(parts[4])
                self.dropped_times = int(parts[7])

        if self.processed is None or\
           self.dropped is None or\
           self.dropped_times is None:
            lprint("-- ERROR while parsing monitor output (see log.txt)--")
            log("stdout:")
            log(out)
            log("stderr:")
            log(err)
            raise RuntimeError("Did not find right values")

