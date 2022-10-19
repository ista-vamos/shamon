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

monitor-tessla-pipe.c: bankmonitor-tessla.vl  $(VLCC)
	$(VLCC) bankmon-pipe.vl monitor-tessla-pipe.c
	-clang-format -i monitor-tessla-pipe.c

monitor-tessla-pipe: monitor-tessla.c $(GENCC) intmap.o $(SHAMON_LIB)
	$(GENCC) monitor-tessla-pipe.c intmap.o -lstdc++
	mv monitor monitor-tessla

experiments: ../bank/interact ../bank/bank monitor
	./experiments.sh

clean:
	-rm *.o monitor *.o

.PHONY: clean all
