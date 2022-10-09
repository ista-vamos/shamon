CPPFLAGS=
CFLAGS=-g #-O3
VLCC=python3 ../../compiler/main.py
GENCC=../../gen/compile.sh
SHAMON_LIB=../../core/libshamon-lib.so  # for dependency

all: monitor bank inputs.txt inputs10.txt inputs100.txt

inputs.txt : inputs.py
	python $^ 100000 > $@

inputs10.txt : inputs.py
	python $^ 10 > $@

inputs100.txt : inputs.py
	python $^ 100 > $@

intmap.o: ../../compiler/cfiles/intmap.cpp
	$(CXX) -c $<

$(VLCC):  # empty, just a depenency for monitor
$(GENCC):
$(SHAMON_LIB):

monitor.c: bankmonitor.vl  $(VLCC)
	$(VLCC) bankmonitor.vl monitor.c
	-clang-format -i monitor.c

monitor: monitor.c $(GENCC) intmap.o $(SHAMON_LIB)
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
