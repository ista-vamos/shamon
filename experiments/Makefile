CPPFLAGS=-I.. -I../shmbuf -I../streams -D_POSIX_C_SOURCE200809L
CFLAGS=-O2 -g -fsanitize=address,undefined -std=c11 -lpthread

all: primes interact monitor-stdin monitor-io monitor-shmbuf-funs

primes:
	+make -C primes

monitor-stdin: monitor-stdin.c ../streams/stream-stdin.o ../shamon.a
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@

monitor-io: monitor-io.c ../streams/stream-fds.o ../shamon.a
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@

#monitor-fastbuf-io: monitor-fastbuf-io.c ../streams/stream-fastbuf-io.o ../shamon.a ../fastbuf/shm_monitor.o ../fastbuf/shm_common.c
#	$(CC) $^ $(CFLAGS) -lpthread $(CPPFLAGS) -o $@

monitor-shmbuf-funs: monitor-shmbuf-funs.c ../streams/stream-funs.o ../shamon.a\
			../shmbuf/buffer.o ../shmbuf/client.o ../shmbuf/shm.o\
			../drfun/events.c
	$(CC) $^ $(CFLAGS) -lpthread $(CPPFLAGS) -o $@

clean:
	-rm *.o monitor-stdin

.PHONY: clean all primes
