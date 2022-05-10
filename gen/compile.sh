#!/bin/bash

set -x

MONITOR="$1"
GENDIR=$(dirname $0)
SHAMONDIR="$GENDIR/.."

#CFLAGS="-g3 -O3 -flto"
CFLAGS="-g -O0"
CPPFLAGS=-I$SHAMONDIR
LDFLAGS=-lpthread
LIBRARIES="$SHAMONDIR/libshamon-arbiter.a\
           $SHAMONDIR/libshamon-utils.a\
           $SHAMONDIR/libshamon-parallel-queue.a\
           $SHAMONDIR/list.c\
           $SHAMONDIR/shmbuf/libshamon-shmbuf.a\
           $SHAMONDIR/drfun/events.c\
           $SHAMONDIR/streams/libshamon-streams.a"

cc $CFLAGS $CPPFLAGS -o monitor $MONITOR -static $LIBRARIES $LDFLAGS
