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


def parse_yml_input(path):
    try:
        from yaml import safe_load as yaml_safe_load, YAMLError
    except ImportError:
        warn("Cannot import from YAML package")
        return None

    with open(path, "r") as stream:
        try:
            spec = yaml_safe_load(stream)
        except YAMLError as exc:
            warn(exc)
            return None
    return spec

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

if __name__ == "__main__":
    from sys import stdout
    gen_header(stdout)
    gen_random_bytes(stdout, 100)
    gen_verifier_nondet(stdout, "int")
    gen_verifier_nondet(stdout, "uint")
    gen_verifier_nondet(stdout, "long")
    gen_verifier_nondet(stdout, "ulong")
    gen_verifier_nondet(stdout, "char")
    gen_verifier_nondet(stdout, "uchar")
    gen_verifier_nondet(stdout, "bool")
    gen_verifier_nondet(stdout, "_Bool")
