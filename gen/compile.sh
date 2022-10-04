#!/bin/bash

set -x

CURDIR="$(pwd)"
MONITORSRC="$1"
shift  # consume the first argument in case there are some additional ones
       # for the compilation
ARBITER_BUFSIZE="$1"
shift

set -e

GENDIR=$(dirname $0)
SHAMONDIR="$GENDIR/.."

CPPFLAGS="-D_POSIX_C_SOURCE=200809L -I${GENDIR} -I$SHAMONDIR\
	   -I$SHAMONDIR/streams -I$SHAMONDIR/core -I$SHAMONDIR/shmbuf"
if grep -q 'CMAKE_BUILD_TYPE.*=Debug' $GENDIR/../CMakeCache.txt; then
	CFLAGS="-g -O0"
	# CFLAGS="$CFLAGS -fsanitize=address,undefined"
else
	CFLAGS="-g3 -O3 -flto  -fno-fat-lto-objects -fPIC -std=c11"
        CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

LDFLAGS=-lpthread
LIBRARIES="$SHAMONDIR/core/libshamon-arbiter.a\
           $SHAMONDIR/core/libshamon-monitor.a\
           $SHAMONDIR/core/libshamon-utils.a\
           $SHAMONDIR/core/libshamon-parallel-queue.a\
           $SHAMONDIR/core/list.c\
           $SHAMONDIR/core/signatures.c\
           $SHAMONDIR/shmbuf/libshamon-shmbuf.a\
           $SHAMONDIR/streams/libshamon-streams.a"

test -z $CC && CC=cc
${CC} $CFLAGS $CPPFLAGS -o $CURDIR/monitor $MONITORSRC $@ $LIBRARIES $LDFLAGS -DSHMBUF_ARBITER_BUFSIZE=$ARBITER_BUFSIZE
