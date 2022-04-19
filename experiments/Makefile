CPPFLAGS=-I..
CFLAGS=-O2 -g -fsanitize=address,undefined

all: primes interact monitor-stdin monitor-io monitor-fastbuf-io

primes:
	make -C primes

monitor-stdin: monitor-stdin.c ../streams/stream-stdin.o ../events.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -I../streams  -o $@

monitor-io: monitor-io.c ../streams/stream-fds.o ../events.o ../queue.c
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -I../streams  -o $@

monitor-fastbuf-io: monitor-fastbuf-io.c ../streams/stream-fastbuf-io.o ../events.o ../fastbuf/shm_monitor.o ../fastbuf/shm_common.c
	$(CC) $^ $(CFLAGS) -lpthread $(CPPFLAGS) -I../streams  -o $@


clean:
	-rm *.o monitor-stdin

.PHONY: clean all primes
