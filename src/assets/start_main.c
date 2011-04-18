int main(int argc, char **argv) {
  struct env_main main;
  void* env = &main;
  union Value dest;
  unsigned int i;
  union Value* right_positional_args = NULL;
  unsigned int right_positional_args_size = 0;
  union Value* left_positional_args = NULL;
  unsigned int left_positional_args_size = 0;
  union Value continuation;

  GC_INIT();

  dest.t = NIL;
  continuation.t = NIL;

  main.c_continuation.t = CLOSURE;
  main.c_continuation.closure.func = &&halt;
  main.c_continuation.closure.env = NULL;

  main.c_null.t = NIL;
  main.c_true.t = BOOLEAN;
  main.c_true.boolean.value = true;
  main.c_false.t = BOOLEAN;
  main.c_false.boolean.value = false;

#define DEFINE_BUILTIN(name) \
  main.c_##name.t = CLOSURE; \
  main.c_##name.closure.func = &&c_##name; \
  main.c_##name.closure.env = NULL;

  DEFINE_BUILTIN(print)
  DEFINE_BUILTIN(if)
  DEFINE_BUILTIN(lessthan)
  DEFINE_BUILTIN(equals)
  DEFINE_BUILTIN(add)

#undef DEFINE_BUILTIN

  goto start;

#define MIN_RIGHT_ARGS(count) \
  if(right_positional_args_size < count) { \
    printf("function takes at least " #count " right arguments, %d given.\n", \
        right_positional_args_size); \
    exit(1); \
  }
#define MAX_RIGHT_ARGS(count) \
  if(right_positional_args_size > count) { \
    printf("function takes at most " #count " right arguments, %d given.\n", \
        right_positional_args_size); \
    exit(1); \
  }
#define MIN_LEFT_ARGS(count) \
  if(left_positional_args_size < count) { \
    printf("function takes at least " #count " left arguments, %d given.\n", \
        left_positional_args_size); \
    exit(1); \
  }
#define MAX_LEFT_ARGS(count) \
  if(left_positional_args_size > count) { \
    printf("function takes at most " #count " left arguments, %d given.\n", \
        left_positional_args_size); \
    exit(1); \
  }
#define REQUIRED_FUNCTION(func) \
  if(func.t != CLOSURE) { \
    printf("cannot call a non-function!\n"); \
    dump_value(func); \
    exit(1); \
  }
#define CALL_FUNC(callable) \
  env = callable.closure.env; \
  goto *callable.closure.func;

c_print:
  REQUIRED_FUNCTION(continuation)
  for(i = 0; i < left_positional_args_size; ++i) {
    print_value(left_positional_args[i]);
    printf(" ");
  }
  for(i = 0; i < right_positional_args_size; ++i) {
    print_value(right_positional_args[i]);
    printf(" ");
  }
  printf("\n");
  if(right_positional_args_size > 0) {
    dest = right_positional_args[0];
  } else if(left_positional_args_size > 0) {
    dest = left_positional_args[left_positional_args_size - 1];
  } else {
    dest.t = NIL;
  }
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  right_positional_args = GC_MALLOC(sizeof(union Value));
  right_positional_args[0] = dest;
  CALL_FUNC(continuation)

c_if:
  REQUIRED_FUNCTION(continuation)
  if(left_positional_args_size == 1 && right_positional_args_size == 1) {
    if(is_true(right_positional_args[0])) {
      REQUIRED_FUNCTION(left_positional_args[0])
      right_positional_args_size = 0;
      left_positional_args_size = 0;
      CALL_FUNC(left_positional_args[0]);
    }
    left_positional_args_size = 0;
    right_positional_args[0].t = NIL;
    CALL_FUNC(continuation)
  }
  MAX_LEFT_ARGS(0)
  MIN_RIGHT_ARGS(2)
  MAX_RIGHT_ARGS(3)
  if(is_true(right_positional_args[0])) {
    REQUIRED_FUNCTION(right_positional_args[1])
    right_positional_args_size = 0;
    CALL_FUNC(right_positional_args[1])
  }
  if(right_positional_args_size >= 3) {
    REQUIRED_FUNCTION(right_positional_args[2])
    right_positional_args_size = 0;
    CALL_FUNC(right_positional_args[2])
  }
  right_positional_args[0].t = NIL;
  right_positional_args_size = 1;
  CALL_FUNC(continuation)

c_lessthan:
  REQUIRED_FUNCTION(continuation)
  if(left_positional_args_size == 1 && right_positional_args_size == 1) {
    right_positional_args[0].boolean.value = builtin_less_than(
        left_positional_args[0], right_positional_args[0]);
  } else {
    MAX_LEFT_ARGS(0)
    MIN_RIGHT_ARGS(2)
    MAX_RIGHT_ARGS(2)
    right_positional_args[0].boolean.value = builtin_less_than(
        right_positional_args[0], right_positional_args[1]);
  }
  right_positional_args[0].t = BOOLEAN;
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  CALL_FUNC(continuation)

c_equals:
  REQUIRED_FUNCTION(continuation)
  if(left_positional_args_size == 1 && right_positional_args_size == 1) {
    right_positional_args[0].boolean.value = builtin_equals(
        left_positional_args[0], right_positional_args[0]);
  } else {
    MAX_LEFT_ARGS(0)
    MIN_RIGHT_ARGS(2)
    MAX_RIGHT_ARGS(2)
    right_positional_args[0].boolean.value = builtin_equals(
        right_positional_args[0], right_positional_args[1]);
  }
  right_positional_args[0].t = BOOLEAN;
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  CALL_FUNC(continuation)

c_add:
  REQUIRED_FUNCTION(continuation)
  if(left_positional_args_size + right_positional_args_size == 0) {
    MIN_RIGHT_ARGS(1)
  }
  if(left_positional_args_size > 0) {
    dest = left_positional_args[0];
    for(i = 1; i < left_positional_args_size; ++i)
      dest = builtin_add(dest, left_positional_args[i]);
    for(i = 0; i < right_positional_args_size; ++i)
      dest = builtin_add(dest, right_positional_args[i]);
  } else {
    dest = right_positional_args[0];
    for(i = 1; i < right_positional_args_size; ++i)
      dest = builtin_add(dest, right_positional_args[i]);
  }
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  right_positional_args = GC_MALLOC(sizeof(union Value));
  right_positional_args[0] = dest;
  CALL_FUNC(continuation)

start:
