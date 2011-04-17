int main(int argc, char **argv) {
  struct env_main main;
  main.c_continuation.t = CLOSURE;
  main.c_continuation.closure.func = &&halt;
  main.c_continuation.closure.env = NULL;
  main.c_null.t = NIL;
  main.c_print.t = CLOSURE;
  main.c_print.closure.func = &&c_print;
  main.c_print.closure.env = NULL;

  void* env = &main;
  union Value dest;
  GC_INIT();

  union Value* right_positional_args = NULL;
  unsigned int right_positional_args_size = 0;
  union Value continuation;
  continuation.t = NIL;

  goto start;

c_print:
  if(right_positional_args_size < 1) {
    printf("function takes 1 arguments, %d given.\n", right_positional_args_size);
    exit(1);
  }
  if(continuation.t != CLOSURE) {
    printf("function requires continuation, none given.\n");
    exit(1);
  }
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
  env = continuation.closure.env;
  goto *continuation.closure.func;

start:
