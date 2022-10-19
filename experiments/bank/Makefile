CPPFLAGS=
CFLAGS=-g #-O3
VLCC=python3 ../../compiler/main.py
GENCC=../../gen/compile.sh
SHAMON_LIB=../../core/libshamon-lib.so  # for dependency

all: monitor-vamos bank interact

intmap.o: ../../compiler/cfiles/intmap.cpp
	$(CXX) -c $<

$(VLCC):  # empty, just a depenency for monitor
$(GENCC):
$(SHAMON_LIB):

interact: interact.c
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -g

monitor.c: bankmonitor.vl  $(VLCC)
	$(VLCC) bankmonitor.vl monitor.c
	-clang-format -i monitor.c

monitor-vamos: monitor.c $(GENCC) intmap.o $(SHAMON_LIB)
	$(GENCC) monitor.c intmap.o -lstdc++
	mv monitor monitor-vamos

bank.o : bank.c bank.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

client.o : client.c bank.h common.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

bank: client.o bank.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lpthread

experiments: interact
	./experiments.sh

clean:
	-rm *.o monitor-vamos bank interact *.o

.PHONY: clean all experiments
