#CFLAGS=-g3 -O3 -flto  -fno-fat-lto-objects -fPIC -std=c11
CFLAGS=-g -fPIC -std=c11
CPPFLAGS=-D_POSIX_C_SOURCE=200809L -DNDEBUG
LDFLAGS=-lpthread

MON_COMPILER_PY=../../compiler/main.py
MON_COMPILER=../../gen/compile.sh

all: monitor

monitor.o: monitor.cpp
	$(CXX) monitor.cpp -g -std=c++17 -c -O3

monitor.c: monitor.txt clean
	python3 $(MON_COMPILER_PY) monitor.txt -b 512 -o monitor.c
	-clang-format -i monitor.c

compiler_utils.o: ../../compiler/cfiles/compiler_utils.c
	$(CC) -g -c $< -o $@

monitor: monitor.c monitor.o compiler_utils.o
	$(MON_COMPILER) monitor.c monitor.o -lstdc++ compiler_utils.o

run: monitor
	./monitor Program:generic:/vrd

experiments: monitor
	./experiments.sh

clean:
	-rm monitor.c monitor.o *.bc *.o vgcore*

.PHONY: all clean


