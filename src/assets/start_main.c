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
  printf("print!\n");
  env = continuation.closure.env;
  goto *continuation.closure.func;

start:
