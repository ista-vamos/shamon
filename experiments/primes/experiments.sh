#!/bin/bash

set -x
set -e

rm -f /tmp/log.txt

cd $(dirname 0)

SRCDIR="$(dirname $0)/../.."
SHM_BUFSIZE_FILE="${SRCDIR}/shmbuf/buffer-size.h"

#for PRIMES_NUM in 10000 20000 30000 40000; do
for PRIMES_NUM in 30000; do
        # size of the SHM buffer in pages
        #for SHM_BUFSIZE in 1 8 16 32; do
        for SHM_BUFSIZE in 1 8 16 32; do
                make clean -j6 -C $SRCDIR
                sed -i "s/#define\\s*SHM_BUFFER_SIZE_PAGES.*/#define SHM_BUFFER_SIZE_PAGES $SHM_BUFSIZE/" $SHM_BUFSIZE_FILE
                make -j6 -C $SRCDIR
                # size of the arbiter buffer in the number of elements
                #for ARBITER_BUFSIZE in 8 16 64 128; do
                for ARBITER_BUFSIZE in 8 64 128 256 512; do
                        ./run.sh $SHM_BUFSIZE $ARBITER_BUFSIZE $PRIMES_NUM
                done
        done
done
