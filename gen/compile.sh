#!/bin/bash

set -x

CURDIR="$(pwd)"
MONITORSRC="$1"
shift  # consume the first argument in case there are some additional ones
       # for the compilation

GENDIR=$(dirname $0)
SHAMONDIR="$GENDIR/.."

CPPFLAGS="-D_POSIX_C_SOURCE=200809L -I$SHAMONDIR -I$SHAMONDIR/streams -I$SHAMONDIR/shmbuf"
if grep -q 'CMAKE_BUILD_TYPE.*=Debug' $CURDIR/../CMakeCache.txt; then
	CFLAGS="-g -O0 -fsanitize=address,undefined"
else
	CFLAGS="-g3 -O3 -flto  -fno-fat-lto-objects -fPIC -std=c11"
        CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

LDFLAGS=-lpthread
LIBRARIES="$SHAMONDIR/libshamon-arbiter.a\
           $SHAMONDIR/libshamon-utils.a\
           $SHAMONDIR/libshamon-parallel-queue.a\
           $SHAMONDIR/list.c\
           $SHAMONDIR/signatures.c\
           $SHAMONDIR/shmbuf/libshamon-shmbuf.a\
           $SHAMONDIR/streams/libshamon-streams.a"

test -z $CC && CC=cc
${CC} $CFLAGS $CPPFLAGS -o ${CURDIR}/monitor $MONITORSRC $@ $LIBRARIES $LDFLAGS
