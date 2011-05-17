int main(int argc, char **argv) {
  struct env_main main;
  void* env = &main;
  union Value dest;
  unsigned int i;
  union Value initial_right_positional_args[10];
  union Value* right_positional_args = initial_right_positional_args;
  unsigned int right_positional_args_size = 0;
  unsigned int right_positional_args_highwater = 10;
  union Value initial_left_positional_args[2];
  union Value* left_positional_args = initial_left_positional_args;
  unsigned int left_positional_args_size = 0;
  unsigned int left_positional_args_highwater = 2;
  union Value continuation;
  union Value hidden_object;
  struct ObjectData hidden_object_data;

  GC_INIT();

  main.c_continuation.t = CLOSURE;
  main.c_continuation.closure.func = &&c_halt;
  main.c_continuation.closure.env = NULL;

  main.c_null.t = NIL;
  main.c_true.t = BOOLEAN;
  main.c_true.boolean.value = true;
  main.c_false.t = BOOLEAN;
  main.c_false.boolean.value = false;

  main.c_hidden_object.t = OBJECT;
  main.c_hidden_object.object.data = &hidden_object_data;
  hidden_object_data.sealed = false;
  hidden_object_data.tree = NULL;

#define DEFINE_BUILTIN(name) \
  main.c_##name.t = CLOSURE; \
  main.c_##name.closure.func = &&c_##name; \
  main.c_##name.closure.env = NULL;

  DEFINE_BUILTIN(print)
  DEFINE_BUILTIN(if)
  DEFINE_BUILTIN(lessthan)
  DEFINE_BUILTIN(equals)
  DEFINE_BUILTIN(add)
  DEFINE_BUILTIN(subtract)
  DEFINE_BUILTIN(divide)
  DEFINE_BUILTIN(multiply)
  DEFINE_BUILTIN(modulo)
  DEFINE_BUILTIN(new_object)
  DEFINE_BUILTIN(seal_object)
  DEFINE_BUILTIN(Array)

#undef DEFINE_BUILTIN

  dest.t = CLOSURE;
  dest.closure.func = &&ho_throw;
  dest.closure.env = NULL;
  set_field(&hidden_object_data, "u_throw", 7, &dest);

  seal_object(&hidden_object_data);

  dest.t = NIL;
  continuation.t = NIL;
  hidden_object.t = NIL;

  goto start;

#define FATAL_ERROR(msg, val) \
  printf("fatal error: %s\n", msg); \
  dump_value(val); \
  return 1;
#define CALL_FUNC(callable) \
  env = callable.closure.env; \
  goto *callable.closure.func;
#define THROW_ERROR(current_hidden_object, val) \
  right_positional_args_size = 1; \
  right_positional_args[0] = val; \
  if(!get_field(current_hidden_object.object.data, "u_throw", 7, &dest)) { \
    FATAL_ERROR("no throw method registered!", main.c_null); \
  } \
  if(dest.t != CLOSURE) { \
    FATAL_ERROR("throw is not a method!", dest); \
  } \
  left_positional_args_size = 0; \
  CALL_FUNC(dest);
#define MIN_RIGHT_ARGS(count) \
  if(right_positional_args_size < count) { \
    THROW_ERROR(hidden_object, make_c_string("function takes at least " #count \
        " right arguments, %d given.", right_positional_args_size)); \
  }
#define MAX_RIGHT_ARGS(count) \
  if(right_positional_args_size > count) { \
    THROW_ERROR(hidden_object, make_c_string("function takes at most " #count \
        " right arguments, %d given.", right_positional_args_size)); \
  }
#define MIN_LEFT_ARGS(count) \
  if(left_positional_args_size < count) { \
    THROW_ERROR(hidden_object, make_c_string("function takes at least " #count \
        " left arguments, %d given.", left_positional_args_size)); \
  }
#define MAX_LEFT_ARGS(count) \
  if(left_positional_args_size > count) { \
    THROW_ERROR(hidden_object, make_c_string("function takes at most " #count \
        " left arguments, %d given.", left_positional_args_size)); \
  }
#define REQUIRED_FUNCTION(func) \
  if(func.t != CLOSURE) { \
    THROW_ERROR(hidden_object, make_c_string("cannot call a non-function!"));\
  }

c_print:
  REQUIRED_FUNCTION(continuation)
  MAX_LEFT_ARGS(0)
  dest.t = NIL;
  for(i = 0; i < right_positional_args_size; ++i) {
    builtin_print(&right_positional_args[i], &dest);
    if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
    printf(" ");
  }
  printf("\n");
  if(right_positional_args_size > 0) {
    dest = right_positional_args[0];
  } else {
    dest.t = NIL;
  }
  right_positional_args_size = 1;
  right_positional_args[0] = dest;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_if:
  REQUIRED_FUNCTION(continuation)
  dest.t = NIL;
  if(left_positional_args_size == 1 && right_positional_args_size == 1) {
    if(builtin_istrue(&right_positional_args[0], &dest)) {
      REQUIRED_FUNCTION(left_positional_args[0])
      right_positional_args_size = 0;
      left_positional_args_size = 0;
      CALL_FUNC(left_positional_args[0]);
    }
    if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
    left_positional_args_size = 0;
    right_positional_args[0].t = NIL;
    dest = continuation;
    continuation.t = NIL;
    CALL_FUNC(dest)
  }
  MAX_LEFT_ARGS(0)
  MIN_RIGHT_ARGS(2)
  MAX_RIGHT_ARGS(3)
  if(builtin_istrue(&right_positional_args[0], &dest)) {
    REQUIRED_FUNCTION(right_positional_args[1])
    right_positional_args_size = 0;
    CALL_FUNC(right_positional_args[1])
  }
  if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
  if(right_positional_args_size == 3) {
    REQUIRED_FUNCTION(right_positional_args[2])
    right_positional_args_size = 0;
    CALL_FUNC(right_positional_args[2])
  }
  right_positional_args[0].t = NIL;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_lessthan:
  REQUIRED_FUNCTION(continuation)
  dest.t = NIL;
  if(left_positional_args_size == 1 && right_positional_args_size == 1) {
    right_positional_args[0].boolean.value = builtin_less_than(
        &left_positional_args[0], &right_positional_args[0], &dest);
  } else {
    MAX_LEFT_ARGS(0)
    MIN_RIGHT_ARGS(2)
    MAX_RIGHT_ARGS(2)
    right_positional_args[0].boolean.value = builtin_less_than(
        &right_positional_args[0], &right_positional_args[1], &dest);
  }
  if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
  right_positional_args[0].t = BOOLEAN;
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_equals:
  REQUIRED_FUNCTION(continuation)
  dest.t = NIL;
  if(left_positional_args_size == 1 && right_positional_args_size == 1) {
    right_positional_args[0].boolean.value = builtin_equals(
        &left_positional_args[0], &right_positional_args[0], &dest);
  } else {
    MAX_LEFT_ARGS(0)
    MIN_RIGHT_ARGS(2)
    right_positional_args[0].boolean.value = builtin_equals(
        &right_positional_args[0], &right_positional_args[1], &dest);
    for(i = 2; i < right_positional_args_size &&
        right_positional_args[0].boolean.value; ++i) {
      if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
      right_positional_args[0].boolean.value = builtin_equals(
          &right_positional_args[1], &right_positional_args[i], &dest);
    }
  }
  if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
  right_positional_args[0].t = BOOLEAN;
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_add:
  REQUIRED_FUNCTION(continuation)
  if(left_positional_args_size + right_positional_args_size == 0) {
    MIN_RIGHT_ARGS(1)
  }
  dest.t = NIL;
  if(left_positional_args_size > 0) {
    for(i = 1; i < left_positional_args_size; ++i) {
      builtin_add(&left_positional_args[0], &left_positional_args[i],
          &left_positional_args[0], &dest);
      if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
    }
    for(i = 0; i < right_positional_args_size; ++i) {
      builtin_add(&left_positional_args[0], &right_positional_args[i],
          &left_positional_args[0], &dest);
      if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
    }
    right_positional_args[0] = left_positional_args[0];
  } else {
    for(i = 1; i < right_positional_args_size; ++i) {
      builtin_add(&right_positional_args[0], &right_positional_args[i],
          &right_positional_args[0], &dest);
      if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
    }
  }
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_multiply:
  REQUIRED_FUNCTION(continuation)
  if(left_positional_args_size + right_positional_args_size == 0) {
    MIN_RIGHT_ARGS(1)
  }
  dest.t = NIL;
  if(left_positional_args_size > 0) {
    for(i = 1; i < left_positional_args_size; ++i) {
      builtin_multiply(&left_positional_args[0], &left_positional_args[i],
          &left_positional_args[0], &dest);
      if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
    }
    for(i = 0; i < right_positional_args_size; ++i) {
      builtin_multiply(&left_positional_args[0], &right_positional_args[i],
          &left_positional_args[0], &dest);
      if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
    }
    right_positional_args[0] = left_positional_args[0];
  } else {
    for(i = 1; i < right_positional_args_size; ++i) {
      builtin_multiply(&right_positional_args[0], &right_positional_args[i],
          &right_positional_args[0], &dest);
      if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
    }
  }
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_subtract:
  REQUIRED_FUNCTION(continuation)
  dest.t = NIL;
  if(right_positional_args_size == 1) {
    MAX_LEFT_ARGS(1)
    if(left_positional_args_size == 0) {
      left_positional_args[0].t = INTEGER;
      left_positional_args[0].integer.value = 0;
    }
    builtin_subtract(&left_positional_args[0], &right_positional_args[0],
        &right_positional_args[0], &dest);
  } else {
    MAX_LEFT_ARGS(0)
    MIN_RIGHT_ARGS(2)
    MAX_RIGHT_ARGS(2)
    builtin_subtract(&right_positional_args[0], &right_positional_args[1],
        &right_positional_args[0], &dest);
  }
  if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_divide:
  REQUIRED_FUNCTION(continuation)
  dest.t = NIL;
  if(left_positional_args_size == 1 && right_positional_args_size == 1) {
    builtin_divide(&left_positional_args[0], &right_positional_args[0],
        &right_positional_args[0], &dest);
  } else {
    MAX_LEFT_ARGS(0)
    MIN_RIGHT_ARGS(2)
    MAX_RIGHT_ARGS(2)
    builtin_divide(&right_positional_args[0], &right_positional_args[1],
        &right_positional_args[0], &dest);
  }
  if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_modulo:
  REQUIRED_FUNCTION(continuation)
  dest.t = NIL;
  if(left_positional_args_size == 1 && right_positional_args_size == 1) {
    builtin_modulo(&left_positional_args[0], &right_positional_args[0],
        &right_positional_args[0], &dest);
  } else {
    MAX_LEFT_ARGS(0)
    MIN_RIGHT_ARGS(2)
    MAX_RIGHT_ARGS(2)
    builtin_modulo(&right_positional_args[0], &right_positional_args[1],
        &right_positional_args[0], &dest);
  }
  if(dest.t != NIL) { THROW_ERROR(hidden_object, dest); }
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest)

c_new_object:
  REQUIRED_FUNCTION(continuation)
  MAX_LEFT_ARGS(0)
  MAX_RIGHT_ARGS(0)
  right_positional_args_size = 1;
  make_object(&right_positional_args[0]);
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

c_seal_object:
  REQUIRED_FUNCTION(continuation)
  MAX_LEFT_ARGS(0)
  MIN_RIGHT_ARGS(1)
  MAX_RIGHT_ARGS(1)
  switch(right_positional_args[0].t) {
    default:
      THROW_ERROR(hidden_object, make_c_string("cannot seal non-object!"));
    case OBJECT:
      seal_object(right_positional_args[0].object.data);
  }
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

ho_throw:
  MAX_LEFT_ARGS(0)
  MAX_RIGHT_ARGS(1)
  MIN_RIGHT_ARGS(1)
  printf("Exception thrown!\n => ");
  dest.t = NIL;
  builtin_print(&right_positional_args[0], &dest);
  if(dest.t != NIL) { FATAL_ERROR("unable to print!", dest); }
  printf("\n");
  return 1;

c_halt:
  MAX_LEFT_ARGS(0)
  MAX_RIGHT_ARGS(1)
  MIN_RIGHT_ARGS(1)
  switch(right_positional_args[0].t) {
    case INTEGER:
      return right_positional_args[0].integer.value;
    case NIL:
      return 0;
    default:
      dest.t = NIL;
      if(builtin_istrue(&right_positional_args[0], &dest))
        return 0;
      if(dest.t != NIL) { FATAL_ERROR("failed exiting with value", dest); }
      return 1;
  }

c_Array:
  MAX_LEFT_ARGS(0)
  REQUIRED_FUNCTION(continuation)
  env = make_array();
  append_values(env, right_positional_args, right_positional_args_size);
  make_object(&right_positional_args[0]);
  dest.t = CLOSURE;
  dest.closure.env = env;
  dest.closure.func = &&c_Array_size;
  set_field(right_positional_args[0].object.data, "u_size", 6, &dest);
  dest.closure.func = &&c_Array_append;
  set_field(right_positional_args[0].object.data, "u_append", 8, &dest);
  dest.closure.func = &&c_Array_pop;
  set_field(right_positional_args[0].object.data, "u_pop", 5, &dest);
  dest.closure.func = &&c_Array_shift;
  set_field(right_positional_args[0].object.data, "u_shift", 7, &dest);
  dest.closure.func = &&c_Array_unshift;
  set_field(right_positional_args[0].object.data, "u_unshift", 9, &dest);
  dest.closure.func = &&c_Array_update;
  set_field(right_positional_args[0].object.data, "s_update_0", 10, &dest);
  dest.closure.func = &&c_Array_index;
  set_field(right_positional_args[0].object.data, "s_index_1", 9, &dest);
  seal_object(right_positional_args[0].object.data);
  left_positional_args_size = 0;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

c_Array_size:
  MAX_LEFT_ARGS(0)
  MAX_RIGHT_ARGS(0)
  REQUIRED_FUNCTION(continuation)
  right_positional_args[0].t = INTEGER;
  right_positional_args[0].integer.value = ((struct Array*)env)->size;
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

c_Array_update:
  MAX_LEFT_ARGS(0)
  MAX_RIGHT_ARGS(2)
  MIN_RIGHT_ARGS(2)
  REQUIRED_FUNCTION(continuation)
  if(right_positional_args[0].t != INTEGER) {
    THROW_ERROR(hidden_object, make_c_string("array indexing must be done with "
        "an integer!"));
  }
  if(right_positional_args[0].integer.value < 0)
    right_positional_args[0].integer.value += ((struct Array*)env)->size;
  if(right_positional_args[0].integer.value >= ((struct Array*)env)->size
      || right_positional_args[0].integer.value < 0) {
    THROW_ERROR(hidden_object, make_c_string("array index out of bounds!"));
  }
  ((struct Array*)env)->data[right_positional_args[0].integer.value] =
      right_positional_args[1];
  right_positional_args[0] = right_positional_args[1];
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

c_Array_index:
  MAX_LEFT_ARGS(0)
  MAX_RIGHT_ARGS(1)
  MIN_RIGHT_ARGS(1)
  REQUIRED_FUNCTION(continuation)
  if(right_positional_args[0].t != INTEGER) {
    THROW_ERROR(hidden_object, make_c_string("array indexing must be done with "
        "an integer!"));
  }
  if(right_positional_args[0].integer.value < 0)
    right_positional_args[0].integer.value += ((struct Array*)env)->size;
  if(right_positional_args[0].integer.value >= ((struct Array*)env)->size
      || right_positional_args[0].integer.value < 0) {
    THROW_ERROR(hidden_object, make_c_string("array index out of bounds!"));
  }
  right_positional_args[0] =
      ((struct Array*)env)->data[right_positional_args[0].integer.value];
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

c_Array_append:
  MAX_LEFT_ARGS(0)
  MIN_RIGHT_ARGS(1)
  REQUIRED_FUNCTION(continuation)
  append_values(env, right_positional_args, right_positional_args_size);
  right_positional_args_size = 1;
  right_positional_args[0].t = NIL;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

c_Array_pop:
  MAX_LEFT_ARGS(0)
  MAX_RIGHT_ARGS(0)
  REQUIRED_FUNCTION(continuation)
  i = ((struct Array*)env)->size;
  if(i == 0) {
    right_positional_args[0].t = NIL;
  } else {
    ((struct Array*)env)->size = --i;
    right_positional_args[0] = ((struct Array*)env)->data[i];
  }
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

c_Array_shift:
  MAX_LEFT_ARGS(0)
  MAX_RIGHT_ARGS(0)
  REQUIRED_FUNCTION(continuation)
  if(((struct Array*)env)->size == 0) {
    right_positional_args[0].t = NIL;
  } else {
    right_positional_args[0] = ((struct Array*)env)->data[0];
    shift_values(env, -1);
  }
  right_positional_args_size = 1;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

c_Array_unshift:
  MAX_LEFT_ARGS(0)
  MIN_RIGHT_ARGS(1)
  REQUIRED_FUNCTION(continuation)
  shift_values(env, right_positional_args_size);
  for(i = 0; i < right_positional_args_size; ++i)
    ((struct Array*)env)->data[i] = right_positional_args[i];
  right_positional_args_size = 1;
  right_positional_args[0].t = NIL;
  dest = continuation;
  continuation.t = NIL;
  CALL_FUNC(dest);

start:
