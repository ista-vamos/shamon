# python3 ../../../compiler/main.py primes.vamos -o primes.c
# python3 ../../../compiler/main.py bank.vamos -o bank.c
# ../../../gen/compile_primes6.sh primes.c
# mv monitor primes_monitor
# ../../../gen/compile_primes6.sh bank.c
# mv monitor bank_monitor

make -C ../../../experiments/primes primes
make -C ../../../experiments/primes primes-bad