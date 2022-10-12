#!/usr/bin/env python3
# coding: utf-8

from sys import argv
from subprocess import run

def compile_monitor_txt(infile, buffsize):
    assert infile.endswith(".in")
    outname = infile[:-3]
    with open(infile, "r") as infile:
        with open(outname, "w") as outfile:
            for line in infile:
                if "@BUFSIZE" in line:
                    line = line.replace("@BUFSIZE", str(buffsize))
                outfile.write(line)

if __name__ == "__main__":
    compile_monitor_txt(argv[1], argv[2])
    exit(0)
