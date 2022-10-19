CPPFLAGS=
CFLAGS=-g #-O3
VLCC=python3 ../../compiler/main.py
SHAMON_LIB=../../core/libshamon-lib.so  # for dependency

all: monitor ../bank/bank ../bank/interact

../bank/interact:
	make -C ../bank interact

../bank/bank:
	make -C ../bank bank

monitor: bankmonitor.vl.in
	./compile.sh

experiments: ../bank/interact ../bank/bank monitor
	./experiments.sh

clean:
	-rm *.o monitor *.o

.PHONY: clean all
