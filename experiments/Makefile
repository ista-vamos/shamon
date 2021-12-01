CPPFLAGS=-I..
CFLAGS=-g

all: monitor gennums_hardcoded

monitor: monitor.o ../shamon.a
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lrt 

gennums_hardcoded: gennums_hardcoded.o ../shamon.a
	$(CC) $^  $(CFLAGS) $(CPPFLAGS) -o $@ -lrt 

clean:
	-rm *.o monitor gennums_hardcoded

.PHONY: clean all
