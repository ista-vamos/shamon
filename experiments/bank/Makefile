CPPFLAGS=
CFLAGS=-g #-O3
VLCC=python3 ../../compiler/main.py
GENCC=../../gen/compile.sh

all: monitor bank inputs.txt inputs100.txt

inputs.txt : inputs.py
	python $^ 100000 > $@

inputs100.txt : inputs.py
	python $^ 100 > $@

intmap.o: ../../compiler/cfiles/intmap.cpp
	$(CXX) -c $<

monitor: bankmonitor.vl intmap.o
	$(VLCC) bankmonitor.vl monitor.c
	$(GENCC) monitor.c intmap.o -lstdc++

bank.o : bank.c bank.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

client.o : client.c bank.h common.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

bank: client.o bank.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lpthread

clean:
	-rm *.o monitor bank inputs.txt inputs100.txt *.o

.PHONY: clean all
