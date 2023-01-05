These set of experiments compiles a (hand-written) event source that for each
`n` in 1 ... `NUM` (`NUM` is a parameter by default set to 10000000) sends an
event carrying 64-bit value `n`. The monitor just reads the values and nothing
else. The purpose is to measure how many events make it all the way thourgh
arbiter to the monitor.

To run the experiments, type

```
make experiments
```

Each set of measurements is repeated 10 times. To change the number of
repetitions, change the value of `REP` variable in `experiments.sh`.

`NUM` can be modified in `run.py`. Tested arbiter buffers can be set up
in `experiments.sh`.
