#include "../src/assets/header.c"
#include "../src/assets/data_structures.c"
#include "../src/assets/builtins.c"

#define assert(bool) \
  if(!bool) { \
    printf("failure on line %d\n", __LINE__); \
    return 1; \
  }

int main(int argc, char** argv) {
  union Value object1;
  union Value object2;
  union Value object3;
  union Value val;
  val.t = NIL;

  make_object(&object1);
  set_field(object1.object.data, (struct ByteArray){"u_throw", 7}, val);
  seal_object(object1.object.data);

  copy_object(&object1, &object2);
  set_field(object2.object.data, (struct ByteArray){"u_return_2dcont", 15}, val);
  seal_object(object2.object.data);

  copy_object(&object2, &object3);
  set_field(object3.object.data, (struct ByteArray){"u_loop_2dcont", 13}, val);
  seal_object(object3.object.data);

  assert(get_field(object3.object.data,
      (struct ByteArray){"u_throw", 7}, &val));
  assert(get_field(object3.object.data,
      (struct ByteArray){"u_return_2dcont", 15}, &val));
  assert(get_field(object3.object.data,
      (struct ByteArray){"u_loop_2dcont", 13}, &val));

  return 0;
}
