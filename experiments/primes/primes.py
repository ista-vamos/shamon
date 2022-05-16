#!/usr/bin/python3

import sys
import os
from time import process_time

count=1
primes = [2]
current = 3
target = 10
if(len(sys.argv)>1):
	target = int(sys.argv[1])
start = process_time()
if target>0:
	print("#1: 2")
while count < target:
	found = False
	for x in primes:
		if current % x == 0:
			found = True
			break
	if not found:
		primes.append(current)
		count=count+1
		print("#"+str(count)+": "+str(current))
	current=current+1
end = process_time()
print(f"time: {end - start} seconds.", file=sys.stderr)

