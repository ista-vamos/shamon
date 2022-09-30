from measure import *
from csv import writer as csvwriter
from sys import argv
from tempfile import mktemp
from os.path import dirname, basename, abspath, join as pathjoin

SHAMONPATH=abspath(pathjoin(dirname(argv[0]), "../..")) # "/opt/shamon"
# assume that dynamorio is in the same folder as shamon
DRIOPATH=abspath(pathjoin(SHAMONPATH, "..", "dynamorio/"))

DRRUN=f"{DRIOPATH}/build/bin64/drrun"
DRIO=[DRRUN, "-root",  f"{DRIOPATH}/build/",
             "-opt_cleancall", "2", "-opt_speed"]
PRIMESPATH=f"{SHAMONPATH}/experiments/primes"
PRIMESMONSRC=f"{SHAMONPATH}/mmtest/monprimes.c"
EMPTYMONSRC=f"{SHAMONPATH}/mmtest/mmempty.c"
COMPILESH=f"{SHAMONPATH}/gen/compile_primes6.sh"

csvlog = None
csvlogf = None

# check if the monitor is a known monitor
# assert basename(PRIMESMONSRC) in ("mmprimes.c", "monprimes.c", "mmprimes-man.c"), PRIMESMONSRC

def open_csvlog(BS, NUM):
    method = "gen" if PRIMESMONSRC.endswith("mmprimes.c") else "man"
    assert method in ("gen", "man"), method
    csv_name = f"times-{method}-{BS}-{NUM}.csv"

    global csvlogf
    csvlogf = open(csv_name, 'a')

    global csvlog
    csvlog = csvwriter(csvlogf)

def close_csvlog():
    csvlogf.close()

class ParseTime:
    def __init__(self, dbg=False, withwaitstats=True):
        self.waiting = []
        self.times = []
        self.time = 0
        self.waited = 0
        self.dbg = dbg
        self.withwaitstats = withwaitstats

    def parse(self, out, err):
        if self.dbg:
            if out:
               for line in out.splitlines():
                   lprint(f"[stdout] {line}", color=GRAY, end='')
            if err:
                for line in err.splitlines():
                    lprint(f"[stderr] {line}", color=GRAY, end='')

        foundwaited = False
        foundtime = False
        for line in err.splitlines():
            if line.startswith(b'time:'):
                parts = line.split()
                assert len(parts) == 3, parts
                f = float(parts[1])
                assert f > 0
                self.times.append(f)
                self.time += f
                foundtime = True
                #return
            if b'busy waited' in line:
                parts = line.split()
                assert len(parts) == 10, parts
                c = int(parts[8])
                self.waited += c
                self.waiting.append(c)
                foundwaited = True

        if not foundtime or\
            (self.withwaitstats and not foundwaited):
            log("-- ERROR while parsing stderr for time info:",
                "-- ERROR while parsing time (see log.txt)--")
            log(err)
            raise RuntimeError("Did not find right values")

    def report(self, key, msg=None):
        assert self.time > 0, f"Invalid time: {self.time}"
        if msg:
            lprint(msg, end=' ', color="\033[0;32m")
        val = self.time / repeat_num()
        lprint(f"Avg CPU time ({repeat_num()} measurements): {val} seconds.",
               color="\033[0;32m")
        if self.waiting:
            for t, c in zip(self.times, self.waiting):
                csvlog.writerow([key, t, c])
        else:
            assert not self.withwaitstats
            for t in self.times:
                csvlog.writerow([key, t])
        return val

class ParseStats:
    def __init__(self):
        self.stats = []
        self.dl, self.dr = None, None
        self.sl, self.sr = None, None
        self.il, self.ir = None, None
        self.pl, self.pr = None, None
        self.errs = None

    def parse(self, out, err):
        if PRIMESMONSRC.endswith("mmprimes.c"):
            return self._parse_mon_gen(out, err)
        return self._parse_mon_man(out, err)

    def report(self, key, msg=None):
        if PRIMESMONSRC.endswith("mmprimes.c"):
            return self._report_mon_gen(key, msg)
        return self._report_mon_man(key, msg)

    def _parse_mon_man(self, out, err):
        errs = 0
        dl, dr = None, None
        pl, pr = None, None
        sl, sr = None, None

        for line in out.splitlines():
            if line.startswith(b'ERROR'):
                errs += 1
            # LEFT : processed 100000 events (21397 compared, 52293 dropped (in 348 holes), 26310 skipped)
            if line.startswith(b'LEFT'):
                parts = line.split()
                assert len(parts) == 13, parts
                pl = int(parts[4][1:])
                dl = int(parts[6])
                sl = int(parts[11])
            if line.startswith(b'RIGHT'):
                parts = line.split()
                assert len(parts) == 13, parts
                pr = int(parts[4][1:])
                dr = int(parts[6])
                sr = int(parts[11])
        if dl is None or sl is None or pl is None or\
           dl + sl + pl == 0 or dl + sl + pl != dr + sr + pr:
            log(out)
            lprint(f"left: {(dl, sl, pl)}, right: {(dr, sr, pr)}, errs: {errs}")
            lprint("-- ERROR while parsing monitor output (see log.txt)--")
            lprint("Did not find right values", color=RED)
            raise RuntimeError("Did not find right values")
        self.stats.append(((dl, sl, pl),(dr, sr, pr), errs))
        self.dl, self.dr = dl, dr
        self.sl, self.sr = sl, sr
        self.pl, self.pr = pl, pr
        self.errs = errs


    def _report_mon_man(self, key, msg=None):
        assert self.stats
        if msg:
            lprint(msg, end=' ', color="\033[0;32m")
        # FIXME: average is not a good statistics here
        dl = sum(self.stats[i][0][0] for i in range(len(self.stats)))
        dr = sum(self.stats[i][1][0] for i in range(len(self.stats)))
        sl = sum(self.stats[i][0][1] for i in range(len(self.stats)))
        sr = sum(self.stats[i][1][1] for i in range(len(self.stats)))
        pl = sum(self.stats[i][0][2] for i in range(len(self.stats)))
        pr = sum(self.stats[i][1][2] for i in range(len(self.stats)))
        errs = sum(self.stats[i][2] for i in range(len(self.stats)))
        N = repeat_num()
        lprint(\
f"""
Average of {N} measurements:

Left dropped: {dl / N}
Right dropped: {dr / N}
Left skipped: {sl / N}
Right skipped: {sr / N}
Left processed: {pl / N}
Right processed: {pr / N}
Detected errors: {errs/ N}""",
               color=GREEN)

        for Sl, Sr, E in self.stats:
            csvlog.writerow([key, *Sl, *Sr, E])
        return ((dl, sl, pl),(dr, sr, pr), errs)


    def _parse_mon_gen(self, out, err):
        errs = 0
        for line in out.splitlines():
            if line.startswith(b'ERROR'):
                errs += 1
            if line.startswith(b'Done!'):
                parts = line.split()
                assert len(parts) == 25, parts
                dl, dr = int(parts[2]), int(parts[5])
                sl, sr = int(parts[8]), int(parts[11])
                il, ir = int(parts[14]), int(parts[17])
                pl, pr = int(parts[20]), int(parts[23])
                if dl + sl + il + pl == 0 or dl + sl + il + pl != dr + sr + ir + pr:
                    log(out)
                    lprint(f"left: {(dl, sl, il, pl)}, right: {(dr, sr, ir, pr)}, errs: {errs}")
                    lprint("Did not find right values", color=RED)
                #print((dl, sl, il, pl),(dr, sr, ir, pr), errs)
                self.stats.append(((dl, sl, il, pl),(dr, sr, ir, pr), errs))
                self.dl, self.dr = dl, dr
                self.sl, self.sr = sl, sr
                self.il, self.ir = il, ir
                self.pl, self.pr = pl, pr
                self.errs = errs
                return

        lprint("-- ERROR while parsing monitor output (see log.txt)--")
        log(out)
        raise RuntimeError("Did not find right values")

    def _report_mon_gen(self, key, msg=None):
        assert self.stats
        if msg:
            lprint(msg, end=' ', color="\033[0;32m")
        # FIXME: average is not a good statistics here
        dl = sum(self.stats[i][0][0] for i in range(len(self.stats)))
        dr = sum(self.stats[i][1][0] for i in range(len(self.stats)))
        sl = sum(self.stats[i][0][1] for i in range(len(self.stats)))
        sr = sum(self.stats[i][1][1] for i in range(len(self.stats)))
        il = sum(self.stats[i][0][2] for i in range(len(self.stats)))
        ir = sum(self.stats[i][1][2] for i in range(len(self.stats)))
        pl = sum(self.stats[i][0][3] for i in range(len(self.stats)))
        pr = sum(self.stats[i][1][3] for i in range(len(self.stats)))
        errs = sum(self.stats[i][2] for i in range(len(self.stats)))
        N = repeat_num()
        lprint(\
f"""
Average of {N} measurements:

Left dropped: {dl / N}
Right dropped: {dr / N}
Left skipped: {sl / N}
Right skipped: {sr / N}
Left ignored: {il / N}
Right ignored: {ir / N}
Left processed: {pl / N}
Right processed: {pr / N}
Detected errors: {errs/ N}""",
               color=GREEN)

        for Sl, Sr, E in self.stats:
            csvlog.writerow([key, *Sl, *Sr, E])
        return ((dl, sl, il, pl),(dr, sr, ir, pr), errs)

#####################################################################

def rand_shm_name(key):
    if key[0] == '/':
        key = key[1:]
    return mktemp(prefix=f"/{key}.")
