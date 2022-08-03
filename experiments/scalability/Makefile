all: source monitor

CFLAGS=-g3 -O3 -flto  -fno-fat-lto-objects -fPIC -std=c11
CPPFLAGS=-D_POSIX_C_SOURCE=200809L -DNDEBUG
LDFLAGS=-lpthread

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

source.o: source.c
	$(CC) -c source.c -I ../.. -I ../../core $(CFLAGS)

source: source.o
	$(CC) source.o -o source $(SOURCE_LIBS)

monitor.o: monitor.c
	$(CC) -c monitor.c -I ../.. -I ../../core $(CFLAGS)

monitor: monitor.o
	$(CC) monitor.o -o monitor ../../core/vector.c ../../monitors/utils.c\
	      -I../.. -I../../core $(CFLAGS)  $(MONITOR_LIBS)  $(LDFLAGS)

experiments: source monitor
	python3 -O -m compileall *.py
	./experiments.sh

.PHONY clean:
	-rm *.o monitor source
	-rm -rf __pycache__

