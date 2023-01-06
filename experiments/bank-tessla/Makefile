CPPFLAGS=
CFLAGS=-g #-O3
VLCC=python3 ../../compiler/main.py
SHAMON_LIB=../../core/libshamon-lib.so  # for dependency
BUFSIZE=1024
#TESSLA_JAR=../../tessla/tessla.jar
TESSLA_JAR=../tessla/tessla-rust.jar

all: monitor$(BUFSIZE) ../bank/bank ../bank/interact

tesslamon/Cargo.toml: bank.tessla
	test -e $(TESSLA_JAR) || echo "Download TeSSLa Rust bundle from https://www.tessla.io/download/ and rename it to ../tessla/tessla-rust.jar"
	java -jar $(TESSLA_JAR) compile-rust -p tesslamon bank.tessla
	cat vendor.txt >> tesslamon/Cargo.toml
	cp -r vendor tesslamon/vendor

../bank/interact:
	make -C ../bank interact

../bank/bank:
	make -C ../bank bank

bankmonitor-$(BUFSIZE).vl: bankmonitor.vl.in
	sed 's/@BUFSIZE/$(BUFSIZE)/' bankmonitor.vl.in > bankmonitor-$(BUFSIZE).vl

bankmonitor-$(BUFSIZE).c: bankmonitor-$(BUFSIZE).vl tesslamon/Cargo.toml
	python3 ../../compiler/main.py -o bankmonitor-$(BUFSIZE).c -t -d tesslamon bankmonitor-$(BUFSIZE).vl

tesslamon/target/release/libmonitor.a: bankmonitor-$(BUFSIZE).c tesslamon/Cargo.toml
	-cd ./tesslamon; cargo build --release

intmap.o: ../../compiler/cfiles/intmap.cpp ../../compiler/cfiles/intmap.h
	g++ -c $< -o $@

monitor$(BUFSIZE): tesslamon/target/release/libmonitor.a bankmonitor-$(BUFSIZE).c intmap.o
	NOLTO=yes ../../gen/compile.sh bankmonitor-$(BUFSIZE).c intmap.o -lstdc++ tesslamon/target/release/libmonitor.a -ldl
	mv monitor monitor$(BUFSIZE)

experiments: ../bank/interact ../bank/bank
	./experiments.sh

clean:
	-rm bankmonitor-*.c bankmonitor-*.vl *.o monitor* *.o
	-rm out*.txt err*.txt
	-rm -rf tesslamon

.PHONY: clean all
