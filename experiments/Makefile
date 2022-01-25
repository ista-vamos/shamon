CPPFLAGS=-I..
CFLAGS=-O2 -g #-fsanitize=address,undefined

all: monitor gennums_hardcoded interact #gennums_hardcoded-instr

interact: interact.c
	$(CC) $^ -o $@

monitor: monitor.o ../libshamon.a isprime.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm

isprime.o: isprime.c

gennums_hardcoded: gennums_hardcoded.o ../libshamon.a
	$(CC) $^  $(CFLAGS) $(CPPFLAGS) -o $@ -lrt

clean:
	-rm *.o monitor gennums_hardcoded gennums_hardcoded-instr

.PHONY: clean all
