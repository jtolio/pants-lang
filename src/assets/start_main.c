int main(int argc, char **argv) {
  struct env_main main;
  main.c_continuation.t = CLOSURE;
  main.c_continuation.closure.func = &&halt;
  main.c_continuation.closure.env = NULL;
  main.c_null.t = NIL;
  main.c_print.t = CLOSURE;
  main.c_print.closure.func = &&c_print;
  main.c_print.closure.env = NULL;
  main.c_if.t = CLOSURE;
  main.c_if.closure.func = &&c_if;
  main.c_if.closure.env = NULL;

  void* env = &main;
  union Value dest;
  GC_INIT();

  union Value* right_positional_args = NULL;
  unsigned int right_positional_args_size = 0;
  union Value continuation;
  continuation.t = NIL;

  goto start;

#define REQUIRED_RIGHT_ARGS(count) \
  if(right_positional_args_size < count) { \
    printf("function takes " #count " arguments, %d given.\n", \
        right_positional_args_size); \
    exit(1); \
  }
#define REQUIRED_FUNCTION(func) \
  if(func.t != CLOSURE) { \
    printf("cannot call a non-function!\n"); \
    exit(1); \
  }
#define CALL_FUNC(callable) \
  env = callable.closure.env; \
  goto *callable.closure.func;

c_print:
  REQUIRED_RIGHT_ARGS(1)
  REQUIRED_FUNCTION(continuation)
  switch(right_positional_args[0].t) {
    case INTEGER:
      printf("%lld\n", right_positional_args[0].integer.value);
      break;
    case FLOAT:
      printf("%f\n", right_positional_args[0].floating.value);
      break;
    case BOOLEAN:
      if(right_positional_args[0].boolean.value) {
        printf("true\n");
      } else {
        printf("false\n");
      }
      break;
    case NIL:
      printf("null\n");
      break;

    case CLOSURE:
    case STRING:
    case OBJECT:
    default:
      printf("TODO: unimplemented\n");
      break;
  }
  CALL_FUNC(continuation)

c_if:
  REQUIRED_RIGHT_ARGS(2)
  REQUIRED_FUNCTION(continuation)
  if(isTrue(&(right_positional_args[0]))) {
    REQUIRED_FUNCTION(right_positional_args[1])
    right_positional_args_size = 0;
    CALL_FUNC(right_positional_args[1])
  } else {
    right_positional_args[0].t = NIL;
    right_positional_args_size = 1;
    CALL_FUNC(continuation)
  }

start:
