CPPFLAGS=-I.. -I../shmbuf -I../streams -D_POSIX_C_SOURCE200809L
CFLAGS=-O3 -g -std=c11 -lpthread -flto #-fsanitize=address,undefined 

all: primes monitor-shmbuf-funs #monitor-stdin monitor-io monitor-entropy

primes:
	+make -C primes

monitor-stdin: monitor-stdin.c ../streams/stream-stdin.o ../libshamon.a
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@

monitor-io: monitor-io.c ../streams/stream-fds.o ../libshamon.a
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@

monitor-entropy: monitor-entropy.c ../streams/stream-fds.o ../libshamon.a
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@

monitor-shmbuf-funs: monitor-shmbuf-funs.c ../streams/stream-funs.o\
                     ../libshamon.a ../shmbuf/libshamon-shmbuf.a\
                     ../libshamon-utils.a  ../streams/libshamon-streams.a\
		     ../drfun/events.c
	$(CC) $^ $(CFLAGS) -static -lpthread $(CPPFLAGS) -o $@

clean:
	-rm *.o monitor-stdin

.PHONY: clean all primes
