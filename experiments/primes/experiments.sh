#!/bin/bash

set -x
set -e

rm -f /tmp/log.txt

cd $(dirname 0)

source ../setup.sh

SRCDIR="$(dirname $0)/../.."

#for SHM_BUFSIZE in 1 8 32 64; do
for SHM_BUFSIZE in 8; do
        make clean -j  -C $SRCDIR
        #sed -i "s/#define\\s*SHM_BUFFER_SIZE_PAGES.*/#define SHM_BUFFER_SIZE_PAGES $SHM_BUFSIZE/" $SHM_BUFSIZE_FILE
        make -j  -C $SRCDIR
        for ARBITER_BUFSIZE in 128 512 1024 2048; do
		if [ "$PRIMES_10000" = "yes" ] ; then
                        ./run.sh $SHM_BUFSIZE $ARBITER_BUFSIZE 10000
		else
               		for PRIMES_NUM in 10000 20000 30000 40000; do
               		        ./run.sh $SHM_BUFSIZE $ARBITER_BUFSIZE $PRIMES_NUM
               		done
		fi
        done
done
