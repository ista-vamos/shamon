import sys
from parser import parse_program

input_file = sys.argv[1] # second argument should be input file

file = " ".join(open(input_file).readlines())

ast = parse_program(file)
