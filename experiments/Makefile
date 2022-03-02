CPPFLAGS=-I..
CFLAGS=-O2 -g -fsanitize=address,undefined

all: monitor gennums_hardcoded interact monitor-stdin monitor-io #gennums_hardcoded-instr

interact: interact.c
	$(CC) $^ -o $@

monitor: monitor.o ../shmbuf/libshamon.a isprime.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm

monitor-stdin: monitor-stdin.c ../streams/stream-stdin.o ../events.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -I../streams  -o $@

monitor-io: monitor-io.c ../streams/stream-stdin.o ../streams/stream-fds.o ../events.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -I../streams  -o $@

isprime.o: isprime.c

gennums_hardcoded: gennums_hardcoded.o ../shmbuf/libshamon.a
	$(CC) $^  $(CFLAGS) $(CPPFLAGS)  -I../shmbuf  -o $@ -lrt

clean:
	-rm *.o monitor gennums_hardcoded gennums_hardcoded-instr monitor-stdin

.PHONY: clean all
