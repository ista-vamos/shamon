CPPFLAGS=-I..
CFLAGS=-O2 -g #-fsanitize=address,undefined

all: monitor gennums_hardcoded gennums_hardcoded-instr

monitor: monitor.o ../shamon.a isprime.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm

gennums_hardcoded: gennums_hardcoded.o ../shamon.a isprime.o
	$(CC) $^  $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm

gennums_hardcoded-instr: gennums_hardcoded.c ../shamon.a isprime.o
	$(CC) $^  -DINSTR $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm

clean:
	-rm *.o monitor gennums_hardcoded gennums_hardcoded-instr

.PHONY: clean all
