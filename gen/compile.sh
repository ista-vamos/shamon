#!/bin/bash

set -x

CURDIR="$(pwd)"
MONITORSRC="$1"
shift  # consume the first argument in case there are some additional ones
       # for the compilation

GENDIR=$(dirname $0)
SHAMONDIR="$GENDIR/.."

#CFLAGS="-g3 -O3 -flto"
CFLAGS="-g -O0"
CPPFLAGS="-I$SHAMONDIR -I$SHAMONDIR/streams -I$SHAMONDIR/shmbuf"
LDFLAGS=-lpthread
LIBRARIES="$SHAMONDIR/libshamon-arbiter.a\
           $SHAMONDIR/libshamon-utils.a\
           $SHAMONDIR/libshamon-parallel-queue.a\
           $SHAMONDIR/list.c\
           $SHAMONDIR/shmbuf/libshamon-shmbuf.a\
           $SHAMONDIR/drfun/events.c\
           $SHAMONDIR/streams/libshamon-streams.a"

test -z $CC && CC=cc
${CC} $CFLAGS $CPPFLAGS -o ${CURDIR}/monitor $MONITORSRC $@ $LIBRARIES $LDFLAGS
