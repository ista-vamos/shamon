#include <unistd.h>
static size_t __pos = 0;
static const size_t __size = 100;
static unsigned char __bytes[] = {61,202,171,6,235,25,197,13,182,204,224,3,71,227,206,23,39,234,0,194,108,82,175,97,141,253,22,218,246,39,27,175,125,161,116,231,62,120,27,88,87,248,190,45,212,131,93,109,153,7,228,128,121,81,229,248,216,219,23,147,224,74,237,154,247,159,79,176,66,168,0,246,147,234,114,91,201,149,19,131,64,66,206,10,12,72,63,105,79,47,222,36,229,215,250,30,33,103,113,66,};

int __VERIFIER_nondet_int(void) {
  const size_t sz = sizeof(int);
  if ((__pos + sz) <= __size) {
    int tmp = *(int*)(&__bytes[__pos]);
    __pos += sz;
  }
  return 0;
}

unsigned int __VERIFIER_nondet_uint(void) {
  const size_t sz = sizeof(unsigned int);
  if ((__pos + sz) <= __size) {
    unsigned int tmp = *(unsigned int*)(&__bytes[__pos]);
    __pos += sz;
  }
  return 0;
}

long __VERIFIER_nondet_long(void) {
  const size_t sz = sizeof(long);
  if ((__pos + sz) <= __size) {
    long tmp = *(long*)(&__bytes[__pos]);
    __pos += sz;
  }
  return 0;
}

unsigned long __VERIFIER_nondet_ulong(void) {
  const size_t sz = sizeof(unsigned long);
  if ((__pos + sz) <= __size) {
    unsigned long tmp = *(unsigned long*)(&__bytes[__pos]);
    __pos += sz;
  }
  return 0;
}

char __VERIFIER_nondet_char(void) {
  const size_t sz = sizeof(char);
  if ((__pos + sz) <= __size) {
    char tmp = *(char*)(&__bytes[__pos]);
    __pos += sz;
  }
  return 0;
}

unsigned char __VERIFIER_nondet_uchar(void) {
  const size_t sz = sizeof(unsigned char);
  if ((__pos + sz) <= __size) {
    unsigned char tmp = *(unsigned char*)(&__bytes[__pos]);
    __pos += sz;
  }
  return 0;
}

_Bool __VERIFIER_nondet_bool(void) {
  const size_t sz = sizeof(_Bool);
  if ((__pos + sz) <= __size) {
    _Bool tmp = *(_Bool*)(&__bytes[__pos]);
    __pos += sz;
  }
  return 0;
}

_Bool __VERIFIER_nondet__Bool(void) {
  const size_t sz = sizeof(_Bool);
  if ((__pos + sz) <= __size) {
    _Bool tmp = *(_Bool*)(&__bytes[__pos]);
    __pos += sz;
  }
  return 0;
}

