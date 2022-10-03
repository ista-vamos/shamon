#!/usr/bin/python

from os import environ
from sys import argv
from time import sleep
from threading import Thread
from time import clock_gettime, CLOCK_MONOTONIC
from subprocess import Popen, PIPE, DEVNULL, TimeoutExpired

# COLORS for printing
RED="\033[0;31m"
GREEN="\033[0;32m"
GRAY="\033[0;37m"

_logf = None
_debug = False

def open_log(f='log.txt'):
    global _logf
    _logf = open(f, 'w')

def close_log():
    _logf.close()

def log_debug(v):
    global _debug
    _debug = v

def log(msg, outmsg=None, end='\n', color=None):
    global _logf
    print(msg, file=_logf, end=end)
    if _debug:
        print("[DBG] " + msg)
    if outmsg:
        if color:
            print(color, end='')
        print(outmsg, end=end)
        if color:
            print("\033[0m")

def lprint(msg, end='\n', color=None):
    log(msg, msg, end=end, color=color)

class PrintOutErr:
    def parse(self, out, err):
        for line in out.splitlines():
            lprint(f"[stdout] {line}", color=GRAY, end='')
        for line in err.splitlines():
            lprint(f"[stderr] {line}", color=GRAY, end='')

class Command:
    def __init__(self, exe, *args, **kwargs):
        self.cmd = [exe, *args]
        self.parser = None
        self.proc = None
        self.retval = None

        self.stdout = DEVNULL
        self.stderr = PIPE
        if kwargs and "stdout" in kwargs:
            self.stdout = kwargs["stdout"]
        if kwargs and "stderr" in kwargs:
            self.stdout = kwargs["stderr"]

    def withparser(self, parser):
        self.parser = parser
        return self

    def run(self, stdin=None):
        log(f"Command: {self} [stdin: {stdin}, stdout: {self.stdout}, stderr: {self.stderr}")
        self.proc = Popen(self.cmd, stdin=stdin,
                          stdout=self.stdout, stderr=self.stderr)
        return self

    def kill(self):
        self.proc.kill()

    def poll(self):
        return self.proc.poll()

    def communicate(self):
        out, err = self.proc.communicate()
        if self.parser:
            self.parser.parse(out, err)
        self.retval = self.proc.returncode
        return self.retval

    def __str__(self):
        return " ".join(map(lambda s: f"'{s}'" if ' ' in s else s, self.cmd))

class PipedCommands:
    def __init__(self, *cmds, **kwargs):
        self.cmds = [*cmds]
        self.procs = []

        # for the last command
        self.stdout = DEVNULL
        self.stderr = PIPE
        if kwargs and "stdout" in kwargs:
            self.stdout = kwargs["stdout"]
        if kwargs and "stderr" in kwargs:
            self.stdout = kwargs["stderr"]

    def withparser(self, parser):
        raise RuntimeError("Add parser to individual commands...")

    def run(self, stdin=None):
        log(f"PipedCommands: {' | '.join(map(str, self.cmds))}")
        procs = self.procs
        max_n = len(self.cmds) - 1
        for n, c in enumerate(self.cmds):
            c.stdout = self.stdout if n == max_n else PIPE
            c.stderr = self.stderr if n == max_n else PIPE
            c.run(stdin=procs[-1].proc.stdout if n > 0 else stdin)
            procs.append(c)
        return self

    def poll(self):
        return self.procs[-1].poll()

    def kill(self):
        for p in self.procs:
            p.kill()

    def communicate(self):
        ret = None
        for proc in self.procs:
            ret = proc.communicate()
            if ret != 0:
                lprint(f"Subcommand '{proc}' returned {retval}", color="\033[0;31m")
        # return value is from the last process
        return ret

    def __str__(self):
        return " | ".join(map(str, self.cmds))

def _measure(cmds, moncmds = (), pipe=False, timeout=None):
    ts = [0] if pipe else [0]*len(cmds)

    start = clock_gettime(CLOCK_MONOTONIC)
    # --- RUN CLINETS ---
    for cmd in cmds:
        cmd.run()

    # --- RUN MONITORS ---
   #if len(moncmds) > 0:
   #    sleep(0.05)
    for c in moncmds:
        c.run()

    # -- PROCESS OUTPUTS OF CLIENTS --
    for cmd in cmds:
        ret = cmd.communicate()
        if ret != 0:
            log(f"Client {cmd} had errors",
                f"\033[0;31mClient {cmd} had errors\033[0m")

    # -- PROCESS OUTPUTS OF MONITORS --
    for mon in moncmds:
        ret = mon.communicate()
        if ret != 0:
            log(f"Monitor {mon} had errors",
                f"\033[0;31mMonitor {mon} had errors\033[0m")
            print(mon.proc)
    end = clock_gettime(CLOCK_MONOTONIC)
    return end - start

class Finished:
    def __init__(self):
        self.finished = False

def raise_timeout(cmds, moncmds, timeout, finished):
    t = 0
    while t < timeout:
        if finished.finished:
            # lprint("[dbg] timeout thread killed because com. finished")
            return
        t += 0.1
        sleep(0.1)

    for c in cmds:
        c.kill()
    for m in moncmds:
        m.kill()
    lprint(f"TIMEOUT {timeout}s reached!", color=RED)

def measure(name, cmds, moncmds=(), timeout=None):
    log(f"------- {name} ------",
        f"\033[0;34m------- {name} ------\033[0m")
    finished = Finished()
    if timeout is not None:
        Thread(target=raise_timeout, args=(cmds, moncmds, timeout, finished), daemon=True).start()
    duration =  _measure(cmds, moncmds)
    finished.finished = True
    return duration

def compile_monitor(compilesh, primessrc):
    p = Popen([compilesh, primessrc], stdout=PIPE, stderr=PIPE)
    out, err = p.communicate()

    if out:
        log(out)
    if err:
        log(err)

    if p.wait() != 0:
        log(f"-- Compiling monitor failed --",
            f"-- \033[0;31m!! Compiling monitor failed (see log) --\033[0m")
        raise RuntimeError("Compiling monitor failed!")
    else:
        lprint("-- Monitor compiled --")



