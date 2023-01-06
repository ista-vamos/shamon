#!/bin/bash

set -e  # exit on any error

source ../setup.sh

export PYTHON_OPTIMIZE=1
#export PYTHONDONTWRITEBYTECODE=1

for i in `seq 1 $REPEAT`; do
        ./run-c.py $@
done

# for i in `seq 1 $REP`; do
#         ./run-cpy.py $@
# done
# 
# for i in `seq 1 $REP`; do
#         ./run-py.py $@
# done

# for i in `seq 1 $REP`; do
#         ./run-java.py $@
# done
# 
# for i in `seq 1 $REP`; do
#         ./run-cjava.py $@
# done
# 
# for i in `seq 1 $REP`; do
#         ./run-pyjava.py $@
# done
