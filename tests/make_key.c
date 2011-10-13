#include "../src/assets/header.c"
#include "../src/assets/data_structures.c"
#include "../src/assets/builtins.c"

#define assert(bool) \
  if(!bool) { \
    printf("failure on line %d\n", __LINE__); \
    return 1; \
  }

int main(int argc, char** argv) {
  struct ByteArray* key;
  key = make_key(0);
  assert(key->size == 1);
  assert(safe_strcmp(*key, (struct ByteArray){"\0", 1}) == 0);
  key = make_key(1);
  assert(key->size == 1);
  assert(safe_strcmp(*key, (struct ByteArray){"\1", 1}) == 0);
  key = make_key(255);
  assert(key->size == 1);
  assert(safe_strcmp(*key, (struct ByteArray){"\xff", 1}) == 0);
  key = make_key(256);
  assert(key->size == 2);
  assert(safe_strcmp(*key, (struct ByteArray){"\0\1", 2}) == 0);
  key = make_key(257);
  assert(key->size == 2);
  assert(safe_strcmp(*key, (struct ByteArray){"\1\1", 2}) == 0);
  key = make_key(258);
  assert(key->size == 2);
  assert(safe_strcmp(*key, (struct ByteArray){"\2\1", 2}) == 0);
  key = make_key(511);
  assert(key->size == 2);
  assert(safe_strcmp(*key, (struct ByteArray){"\xff\1", 2}) == 0);
  key = make_key(512);
  assert(key->size == 2);
  assert(safe_strcmp(*key, (struct ByteArray){"\0\2", 2}) == 0);
  key = make_key(65535);
  assert(key->size == 2);
  assert(safe_strcmp(*key, (struct ByteArray){"\xff\xff", 2}) == 0);
  key = make_key(65536);
  assert(key->size == 3);
  assert(safe_strcmp(*key, (struct ByteArray){"\0\0\1", 3}) == 0);
  key = make_key(4294967295);
  assert(key->size == 4);
  assert(safe_strcmp(*key, (struct ByteArray){"\xff\xff\xff\xff", 4}) == 0);
  key = make_key(4294967296);
  assert(key->size == 5);
  assert(safe_strcmp(*key, (struct ByteArray){"\0\0\0\0\1", 5}) == 0);

  return 0;
}
