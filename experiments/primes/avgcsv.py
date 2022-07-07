#!/usr/bin/python3

from statistics import mean, stdev
from csv import reader
from sys import argv

data = {}
with open(argv[1], 'r') as f:
    rdr = reader(f)
    for row in rdr:
        if row[0].endswith('-ln'):
            row = [row[0] + '-' + row[1]] + row[2:]
        R = data.get(row[0])
        if R:
            assert len(R) == len(row) - 1, row
            for i in range(len(row) - 1):
                R[i].append(float(row[i+1]))
        else:
            data[row[0]] = [[float(x)] for x in row[1:]]

def var(V):
    if len(V) < 2:
        return -1
    return stdev(V)

srtd = sorted(list(data.items()), key=lambda x: x[0])
for k, vals in srtd:
    assert all(map(lambda V: len(V) == len(vals[0]), vals))
    print(f"[n={len(vals[0])}]", k, ": ", ", ".join(map(lambda V: f"{mean(V):.3f}±{var(V):.3f}", vals)))
