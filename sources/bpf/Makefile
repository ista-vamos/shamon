CC=clang

all: program.bpf loader

program.bpf: program.bpf.c
	$(CC) $(CFLAGS) -target bpf -g -c $< -o program.bpf

loader: loader.c
	$(CC) -g -lbpf loader.c -o loader



