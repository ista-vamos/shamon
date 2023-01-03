all: source monitor

CFLAGS=-g3 -O3 -flto  -fno-fat-lto-objects -fPIC -std=c11
CPPFLAGS=-D_POSIX_C_SOURCE=200809L -DNDEBUG
LDFLAGS=-lpthread

MON_COMPILER_PY=../../compiler/main.py
MON_COMPILER=../../gen/compile.sh

SOURCE_LIBS=../../shmbuf/libshamon-shmbuf.a\
            ../../core/libshamon-source.a\
            ../../core/libshamon-signature.a\
            ../../core/libshamon-event.a\
            ../../core/libshamon-list.a

MONITOR_LIBS= ../../core/libshamon-signature.a\
              ../../core/libshamon.a\
              ../../core/libshamon-arbiter.a\
              ../../core/libshamon-parallel-queue.a\
              ../../core/libshamon-event.a\
              ../../streams/libshamon-streams.a\
	      ../../shmbuf/libshamon-shmbuf.a

source.o: source.c
	$(CC) -c source.c -I ../.. -I ../../core $(CFLAGS)

source: source.o
	$(CC) source.o -o source $(SOURCE_LIBS)

monitor.c: monitor.txt
	python3 $(MON_COMPILER_PY) monitor.txt -o monitor.c
	-clang-format -i monitor.c

monitor: monitor.c
	$(MON_COMPILER) monitor.c

experiments:
	python3 -O -m compileall *.py
	./experiments.sh

.PHONY clean:
	-rm monitor.c monitor.txt
	-rm *.o monitor source
	-rm -rf __pycache__

