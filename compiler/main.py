import sys
from parser import parse_program
from type_checker import TypeChecker

input_file = sys.argv[1] # second argument should be input file

file = " ".join(open(input_file).readlines())

# Type checker initialization
# TypeChecker.clean_checker()
TypeChecker.add_reserved_keywords()


ast = parse_program(file)
