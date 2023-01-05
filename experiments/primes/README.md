To run the experiments, type

```
make experiments
```

Each set of measurements is repeated 10 times. To change the number of
repetitions, change the value of `REP` variable in `run.sh`.
The number of generated primes can be set by modifying the variable
`NUM` in `run.py` (or passing it as an argument to that script).

To run experiments with TeSSLa monitor, type
```
make experiments-tessla
```

Each set of measurements is repeated 10 times. To change the number of
repetitions, change the value of `REP` variable in `experiments-tessla.sh`. The
number of generated primes can be changed by adjusting the limit of the counter
variable `PRIMES_NUM` (on multiple locations) in the same script.

