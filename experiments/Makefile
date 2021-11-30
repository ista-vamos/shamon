CPPFLAGS=-I..

all: monitor gennums_hardcoded

monitor: monitor.o ../shamon.a
	$(CC) $^  -o $@ -lrt 

gennums_hardcoded: monitor.o ../shamon.a
	$(CC) $^  -o $@ -lrt 

clean:
	-rm *.o monitor gennums_hardcoded

.PHONY: clean all
