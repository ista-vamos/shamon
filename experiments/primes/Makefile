CPPFLAGS=-I../..
CFLAGS=-O2 -g -fsanitize=address,undefined

all: monitor-primes gennums_hardcoded 

monitor-primes: monitor-primes.o ../../shmbuf/libshamon.a isprime.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm

isprime.o: isprime.c

gennums_hardcoded: gennums_hardcoded.o ../../shmbuf/libshamon.a
	$(CC) $^  $(CFLAGS) $(CPPFLAGS)  -I../../shmbuf  -o $@ -lrt

clean:
	-rm *.o monitor-primes gennums_hardcoded gennums_hardcoded-instr

.PHONY: clean all
