#!/bin/bash

REP=1
export PYTHON_OPTIMIZE=1
export PYTHONDONTWRITEBYTECODE=1

for i in `seq 1 $REP`; do
        ./run.py $@
done
