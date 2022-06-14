all: source monitor
CFLAGS=-g3 -O3 -flto  -fno-fat-lto-objects -fPIC -std=c11
CPPFLAGS=-D_POSIX_C_SOURCE=200809L -DNDEBUG
LDFLAGS=-lpthread

SOURCE_LIBS=../../shmbuf/libshamon-shmbuf.a\
            ../../libshamon-signature.a\
            ../../libshamon-utils.a

MONITOR_LIBS= ../../libshamon-signature.a\
             ../../libshamon.a\
             ../../libshamon-arbiter.a\
             ../../libshamon-parallel-queue.a\
             ../../libshamon-utils.a\
             ../../streams/libshamon-streams.a\
	     ../../shmbuf/libshamon-shmbuf.a

source.o: source.c
	$(CC) -c source.c -I ../..  $(CFLAGS)

source: source.o
	$(CC) source.o -o source $(SOURCE_LIBS)

monitor.o: monitor.c
	$(CC) -c monitor.c -I ../.. $(CFLAGS)

monitor: monitor.o
	$(CC) monitor.o -o monitor ../../vector.c ../utils.c -I../.. $(CFLAGS)  $(MONITOR_LIBS)  $(LDFLAGS)
