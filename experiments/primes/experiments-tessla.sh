#!/bin/bash

set -x
set -e

rm -f /tmp/log.txt

cd $(dirname 0)

SRCDIR="$(readlink -f $(dirname $0)/../..)"
VLCC="python3 $SRCDIR/compiler/main.py"
GENCC="$SRCDIR/gen/compile.sh"
TESSLA_JAR=../tessla/tessla-rust.jar

source ../setup.sh

# compile the tessla monitor
java -jar $TESSLA_JAR compile-rust primes.tessla -b tessla-monitor

PRIMES_NUM=40000
if [ "$PRIMES_10000" = "yes" ]; then
	PRIMES_NUM=10000
fi

for i in `seq 1 $REPEAT`; do
for SHM_BUFSIZE in 8; do
#for SHM_BUFSIZE in 8 16; do
        make clean -j  -C $SRCDIR
        make -j -C $SRCDIR
	make primes

	g++ -O3 -c $SRCDIR/compiler/cfiles/intmap.cpp

	SPEC="primes-tessla.txt"
        for ARBITER_BUFSIZE in 4 8 16 32 64 128 256 512 1024; do

		SPEC_IN="primes-tessla.txt.in"
		# generate monitor spec
	        cpp -P $SPEC_IN -DARBITER_BUFSIZE=$ARBITER_BUFSIZE >$SPEC
		# compile it
		$VLCC $SPEC -b $ARBITER_BUFSIZE -o monitor-tessla.c
		$GENCC monitor-tessla.c
		mv monitor vamos-tessla

                #for PRIMES_NUM in 10000 20000 30000 40000; do
                ./run-tessla.sh $SHM_BUFSIZE $ARBITER_BUFSIZE $PRIMES_NUM "forward"
                ./run-tessla.sh $SHM_BUFSIZE $ARBITER_BUFSIZE $PRIMES_NUM "forward" "bad"
		#done

		SPEC_IN="primes-align-tessla.txt.in"
		# generate monitor spec
		sed s@ARBITER_BUFSIZE@$ARBITER_BUFSIZE@ $SPEC_IN > $SPEC
		sed -i s@SRCDIR@$SRCDIR@ $SPEC

		# compile it
		$VLCC $SPEC -b $ARBITER_BUFSIZE -o monitor-tessla.c
		$GENCC monitor-tessla.c ./intmap.o $SRCDIR/compiler/cfiles/compiler_utils.c -lstdc++
		mv monitor vamos-tessla

                #for PRIMES_NUM in 10000 20000 30000 40000; do
                ./run-tessla.sh $SHM_BUFSIZE $ARBITER_BUFSIZE $PRIMES_NUM "align"
                ./run-tessla.sh $SHM_BUFSIZE $ARBITER_BUFSIZE $PRIMES_NUM "align" "bad"
		#done


		SPEC_IN="primes-align2-tessla.txt.in"
		# generate monitor spec
		sed s@ARBITER_BUFSIZE@$ARBITER_BUFSIZE@ $SPEC_IN > $SPEC
		# compile it
		$VLCC $SPEC -b $ARBITER_BUFSIZE -o monitor-tessla.c
		$GENCC monitor-tessla.c
		mv monitor vamos-tessla

                # for PRIMES_NUM in 10000 20000 30000 40000; do
                ./run-tessla.sh $SHM_BUFSIZE $ARBITER_BUFSIZE $PRIMES_NUM "alternate"
                ./run-tessla.sh $SHM_BUFSIZE $ARBITER_BUFSIZE $PRIMES_NUM "alternate" "bad"
		#done

        done
done
done
