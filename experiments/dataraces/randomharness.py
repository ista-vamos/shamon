import sys
from random import getrandbits

nondets = {
    "int" : "int",
    "uint" : "unsigned int",
    "long" : "long",
    "ulong" : "unsigned long",
    "bool" : "_Bool",
    "_Bool" : "_Bool",
    "char" : "char",
    "uchar" : "unsigned char",
}

def get_rand_byte():
    return getrandbits(8)

def gen_random_bytes(stream, num):
    assert num > 0
    stream.write("static size_t __pos = 0;\n")
    stream.write(f"static const size_t __size = {num};\n")
    stream.write("static unsigned char __bytes[] = {")
    for i in range(num):
        stream.write(f"{get_rand_byte()},")
    stream.write("};\n\n")


def gen_verifier_nondet(stream, name):
    retty = nondets[name]
    stream.write(f"{retty} __VERIFIER_nondet_{name}(void) {{\n")
    stream.write(f"  const size_t sz = sizeof({retty});\n")
    stream.write(f"  if ((__pos + sz) <= __size) {{\n")
    stream.write(f"    {retty} tmp = *({retty}*)(&__bytes[__pos]);\n")
    stream.write( "    __pos += sz;\n  }\n")
    stream.write( "  return 0;\n")
    stream.write("}\n\n")

def gen_header(stream):
    stream.write("#include <unistd.h>\n")

def gen_harness(stream, n):
    gen_header(stream)
    gen_random_bytes(stream, n)
    gen_verifier_nondet(stream, "int")
    gen_verifier_nondet(stream, "uint")
    gen_verifier_nondet(stream, "long")
    gen_verifier_nondet(stream, "ulong")
    gen_verifier_nondet(stream, "char")
    gen_verifier_nondet(stream, "uchar")
    gen_verifier_nondet(stream, "bool")
    gen_verifier_nondet(stream, "_Bool")

if __name__ == "__main__":
    N = 100
    if "-o" in sys.argv:
        out_idx = sys.argv.index("-o")
        with open(sys.argv[out_idx + 1], "w") as stream:
            gen_harness(stream, N)
    else:
        gen_harness(sys.stdout, N)

