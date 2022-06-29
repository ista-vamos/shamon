CPPFLAGS=
CFLAGS=-O3

all: experiments

primes: primes.c

primes-bad: primes-bad.c

primes-2: primes-2.c

experiments: primes primes-bad
	./experiments.sh

clean:
	-rm *.o primes primes-2

.PHONY: clean all experiments
