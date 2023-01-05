CPPFLAGS=
CFLAGS=-O3

all: prepare

../../compiler/cfiles/compiler_utils.o: ../../compiler/cfiles/compiler_utils.c
	$(CC) -c $< -o $@

../../compiler/cfiles/intmap.o: ../../compiler/cfiles/intmap.cpp
	$(CXX) -c $< -o $@

prepare: primes primes-bad primes-python ../../compiler/cfiles/compiler_utils.o ../../compiler/cfiles/intmap.o
	python3 shm_prepare_programs.py

primes: primes.c

primes-bad: primes-bad.c

#primes-2: primes-2.c

#primes-java: primes.java
#        javac -Werror $<

primes-python: primes.py
	python3 -O -m compileall *.py
	#python3 -O -m compileall -o 1 *.py

experiments: prepare
	./experiments.sh

experiments-tessla:
	./experiments-tessla.sh

clean:
	-rm *.o primes primes-2 primes-bad
	-rm -rf __pycache__

.PHONY: clean all experiments experiments-tessla
