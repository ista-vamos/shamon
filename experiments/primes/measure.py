#!/usr/bin/python

from os import environ
from sys import argv
from time import sleep
from subprocess import Popen, PIPE, DEVNULL


REPEAT_NUM=10

def set_repeat_num(n):
    global REPEAT_NUM
    REPEAT_NUM = n
    
def repeat_num():
    return REPEAT_NUM

# COLORS for printing
RED="\033[0;31m"
GREEN="\033[0;32m"

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
            lprint(f"[stdout] {line}", color="\033[0;37m", end='')
        for line in err.splitlines():
            lprint(f"[stderr] {line}", color="\033[0;33m", end='')
    
class Command:
    def __init__(self, exe, *args):
        self.cmd = [exe, *args]
        self.parser = None
        self.proc = None
        self.retval = None
    
    def withparser(self, parser):
        self.parser = parser
        return self
    
    def run(self, stdout=PIPE, stderr=PIPE, stdin=None):
        log(f"Command: {self}")
        self.proc = Popen(self.cmd, stdout=stdout, stderr=stderr, stdin=stdin)
        return self
    
    def communicate(self):
        out, err = self.proc.communicate()
        if self.parser:
            self.parser.parse(out, err)
        self.retval = self.proc.returncode
        return self.retval
    
    def __str__(self):
        return " ".join(map(lambda s: f"'{s}'" if ' ' in s else s, self.cmd))

class PipedCommands:
    def __init__(self, *cmds):
        self.cmds = [*cmds]
        self.procs = []
    
    def withparser(self, parser):
        raise RuntimeError("Add parser to individual commands...")
    
    def run(self, stdout=PIPE, stderr=PIPE):
        log(f"PipedCommands: {' | '.join(map(str, self.cmds))}")
        procs = self.procs
        for n, c in enumerate(self.cmds):
            c.run(stdin=procs[-1].proc.stdout if n > 0 else None)
            procs.append(c)
        return self
    
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

def _measure(cmds, moncmds = (), pipe=False):
    ts = [0] if pipe else [0]*len(cmds)
    for i in range(REPEAT_NUM):
       
        # --- RUN CLINETS ---
        for cmd in cmds:
            cmd.run()                
            
        # --- RUN MONITORS ---    
        if len(moncmds) > 0:
            sleep(0.15)
        for c in moncmds:
            c.run()

        # -- PROCESS OUTPUTS OF MONITORS --
        for mon in moncmds:
            ret = mon.communicate()
            if ret != 0:
                log(f"Monitor {mon} had errors",
                    f"\033[0;31mMonitor {mon} had errors\033[0m")
                print(mon.proc)

        # -- PROCESS OUTPUTS OF CLIENTS --
        for cmd in cmds:
            ret = cmd.communicate()
            if ret != 0:
                log(f"Client {cmd} had errors",
                    f"\033[0;31mClient {cmd} had errors\033[0m")


def measure(name, cmds, moncmds=()):
    log(f"------- {name} ------",
        f"\033[0;34m------- {name} ------\033[0m")
    _measure(cmds, moncmds)

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
    else:
        lprint("-- Monitor compiled --")



