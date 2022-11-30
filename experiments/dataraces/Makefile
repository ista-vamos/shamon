CFLAGS=-g3 -O3 -flto  -fno-fat-lto-objects -fPIC -std=c11
CPPFLAGS=-D_POSIX_C_SOURCE=200809L -DNDEBUG
LDFLAGS=-lpthread

MON_COMPILER_PY=../../compiler/main.py
MON_COMPILER=../../gen/compile.sh

SOURCE_LIBS=../../shmbuf/libshamon-shmbuf.a\
            ../../core/libshamon-signature.a\
            ../../core/libshamon-utils.a

MONITOR_LIBS= ../../core/libshamon-signature.a\
              ../../core/libshamon.a\
              ../../core/libshamon-arbiter.a\
              ../../core/libshamon-parallel-queue.a\
              ../../core/libshamon-utils.a\
              ../../streams/libshamon-streams.a\
	      ../../shmbuf/libshamon-shmbuf.a

all: monitor

monitor.o: monitor.cpp
	clang++ monitor.cpp -g -std=c++17 -c -O3

monitor.c: monitor.txt
	python3 $(MON_COMPILER_PY) monitor.txt -b 512 -o monitor.c
	-clang-format -i monitor.c

monitor: monitor.c monitor.o
	$(MON_COMPILER) monitor.c monitor.o


