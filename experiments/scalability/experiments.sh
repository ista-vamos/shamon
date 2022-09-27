#!/bin/bash

set -e

cd $(dirname 0)

rm -f log.txt

SRCDIR="$(dirname $0)/../.."
SHM_BUFSIZE_FILE="${SRCDIR}/shmbuf/buffer-size.h"


for SHM_BUFSIZE in 1 8 16 32; do
	make clean
        make clean -j -C $SRCDIR
        sed -i "s/#define\\s*SHM_BUFFER_SIZE_PAGES.*/#define SHM_BUFFER_SIZE_PAGES $SHM_BUFSIZE/" $SHM_BUFSIZE_FILE
        make -j -C $SRCDIR
	make source -B
        ./run.sh $SHM_BUFSIZE
done
