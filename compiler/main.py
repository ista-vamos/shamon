import sys
from parser import parse_program

# input_file = sys.argv[1] # second argument should be input file

# file = " ".join(open(input_file).readlines())

input = """
stream type Primes
{
    Prime(n : int, p : int);  
}

stream type PrimeEvents
{
    LPrime(n : int, p : int);
    RPrime(n : int, p : int);
    LSkip(n : int);
    RSkip(n : int);
}
event source Left : Primes -> autodrop(8) Primes
{
    on Prime(n, p) forward Prime(n,p);
}
event source Right : Primes -> autodrop(8) Primes
{
    on Prime(n, p) forward Prime(n,p);
}
"""

parse_program(input)