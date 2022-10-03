# Compiler for VAMOS

This is a compiler for a best-effort third-party monitoring middleware.
It produces a C file.

## Requirements
Python 3 (we use Python 3.8.9).

## Usage

Execute main and pass as first argument the input file, and as second argument the output file. For example

```bash
python main.py tests/short_primes.txt monitor.c
```
