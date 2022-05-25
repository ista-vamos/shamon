#!/usr/bin/python3

from sys import argv

chdir(WORKINGDIR)

print(f"-- Working directory is {WORKINGDIR} --", file=log)
print(f"-- Working directory is {WORKINGDIR} --")
print(f"-- Log: {WORKINGDIR}/log.txt --")

print("-- Compiling monitor --")
print("-- Compiling monitor --", file=log)

measure("'Differential monitor for primes' piping",
        [([f"{PRIMESPATH}/primes-bad", NUM, str(int(NUM)/10)],
          [f"{SHAMONPATH}/sources/regex",
           "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"]),
         ([f"{PRIMESPATH}/primes", NUM],
          [f"{SHAMONPATH}/sources/regex",
           "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"])
        ],
        [["./monitor",
         "Left:drregex:/primes1", "Right:drregex:/primes2"]],
         msg="First C, second Python")
exit(0)

c_native =\
measure("'C primes' alone", [[f"{PRIMESPATH}/primes", NUM]])
c_drio =\
measure("'C primes' DynamoRIO (no monitor)",
        [DRIO + ["--", f"{PRIMESPATH}/primes", NUM]])
measure("'C primes' piping (monitor reads and drops)",
        [([f"{PRIMESPATH}/primes", NUM],
          [f"{SHAMONPATH}/sources/regex",
            "/primes", "prime", "#([0-9]+): ([0-9]+)", "ii"])
        ],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:regex:/primes"]])
measure("'C primes' DynamoRIO (monitor reads and drops)",
        [DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", f"{PRIMESPATH}/primes", NUM]],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:drregex:/primes1"]])
print('------------------')
python_native =\
measure("'Python primes' alone", [[f"{PRIMESPATH}/primes.py", NUM]])
python_drio =\
measure("'Python primes' DynamoRIO (no monitor)",
        [DRIO + ["--", "python3", f"{PRIMESPATH}/primes.py", NUM]])
measure("'Python primes' piping (monitor reads and drops)",
        [(["python3", f"{PRIMESPATH}/primes.py", NUM],
          [f"{SHAMONPATH}/sources/regex",
           "/primes", "prime", "#([0-9]+): ([0-9]+)", "ii"])
        ],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:regex:/primes"]])
measure("'Python primes' DynamoRIO (monitor reads and drops)",
        [DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", "python3", f"{PRIMESPATH}/primes.py", NUM]],
        [[f"{SHAMONPATH}/experiments/monitor-consume",
         "primes:drregex:/primes1"]])
print('------------------')
dm_pipes =\
measure("'Differential monitor for primes' piping",
        [([f"{PRIMESPATH}/primes", NUM],
          [f"{SHAMONPATH}/sources/regex",
           "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"]),
         (["python3", f"{PRIMESPATH}/primes.py", NUM],
          [f"{SHAMONPATH}/sources/regex",
           "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"])
        ],
        [["./monitor",
         "Left:drregex:/primes1", "Right:drregex:/primes2"]],
         msg="First C, second Python")
dm_drio =\
measure("'Differential monitor for primes' DynamoRIO sources",
        [DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes1", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", f"{PRIMESPATH}/primes", NUM],
        DRIO +
        ["-c", f"{SHAMONPATH}/sources/drregex/libdrregex.so",
         "/primes2", "prime", "#([0-9]+): ([0-9]+)", "ii"] +
        ["--", "python3", f"{PRIMESPATH}/primes.py", NUM]],
        [["./monitor",
         "Left:drregex:/primes1", "Right:drregex:/primes2"]],
         msg="First C, second Python")


print(
f"""
Total slowdown of C program (DynamoRIO): {dm_drio[0]/c_native[0]}.
Total slowdown of Python program (DynamoRIO): {dm_drio[1]/python_native[0]}.
Slowdown caused by instrumentation of C program (DynamoRIO): {dm_drio[0]/c_drio[0]}.
Slowdown caused by instrumentation of Python program (DynamoRIO): {dm_drio[1]/python_drio[0]}.
Total slowdown of C program (pipes): {dm_pipes[0]/c_native[0]}.
Total slowdown of Python program (pipes): {dm_pipes[1]/python_native[0]}.
""")

print(f"-- Removing working directory {WORKINGDIR} --")
print(f"-- Removing working directory {WORKINGDIR} --", file=log)
log.close()
chdir("/")
rmtree(WORKINGDIR)
