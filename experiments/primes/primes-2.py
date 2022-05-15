#!/usr/bin/python3

from sys import argv

NUM = int(argv[1])

def isprime(x):
    for i in range(2, x):
        if x % i == 0:
            return False
    return True

n = 1
for i in range(2, NUM):
    if isprime(i):
        print(f"{n} {i}")
        n += 1
