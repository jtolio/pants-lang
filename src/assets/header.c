#include <stdlib.h>
#include <stdio.h>
#include <gc/gc.h>

#define bool char
#define true 1
#define false 0

enum ValueTag {
  INTEGER,
  FLOAT,
  STRING,
  OBJECT,
  BOOLEAN,
  NIL,
  CLOSURE,
  CELL
};

union Value;

struct Integer {
  enum ValueTag t;
  long long value;
};

struct Float {
  enum ValueTag t;
  double value;
};

struct String {
  enum ValueTag t;
  bool byte_oriented;
  char* value;
  unsigned int value_size;
};

struct ObjectData;

struct Object {
  enum ValueTag t;
  struct ObjectData* data;
};

struct Boolean {
  enum ValueTag t;
  bool value;
};

struct Nil {
  enum ValueTag t;
};

struct Closure {
  enum ValueTag t;
  void* func;
  void* env;
};

struct Cell {
  enum ValueTag t;
  union Value* addr;
};

union Value {
  enum ValueTag t;
  struct Integer integer;
  struct Float floating;
  struct String string;
  struct Object object;
  struct Boolean boolean;
  struct Closure closure;
  struct Cell cell;
};

struct env_main {
  union Value c_continuation;
  union Value c_null;
  union Value c_print;
  union Value c_if;
  union Value c_true;
  union Value c_false;
  union Value c_lessthan;
  union Value c_equals;
  union Value c_add;
};

static inline union Value make_cell(union Value val) {
  union Value v;
  v.t = CELL;
  v.cell.addr = GC_MALLOC(sizeof(union Value));
  *(v.cell.addr) = val;
  return v;
}

static inline int safe_strcmp(char* str1, unsigned int str1_size, char* str2,
    unsigned int str2_size) {
  unsigned int i;
  unsigned int cmp_size = (str1_size > str2_size) ? str2_size : str1_size;
  for(i = 0; i < cmp_size; ++i) {
    if(str1[i] < str2[i]) return -1;
    if(str1[i] > str2[i]) return 1;
  }
  if(str1_size < str2_size) return -1;
  if(str1_size > str2_size) return 1;
  return 0;
}

// for testing
static inline void dump_value(union Value val) {
  switch(val.t) {
    case INTEGER:
      printf("integer: %lld\n", val.integer.value);
      break;
    case FLOAT:
      printf("float: %f\n", val.floating.value);
      break;
    case STRING:
      printf("string\n");
      break;
    case OBJECT:
      printf("object\n");
      break;
    case BOOLEAN:
      printf(val.boolean.value ? "boolean: true\n" : "boolean: false\n");
      break;
    case NIL:
      printf("nil\n");
      break;
    case CLOSURE:
      printf("closure\n");
      break;
    case CELL:
      printf("cell:\n");
      dump_value(*val.cell.addr);
      break;
    default:
      printf("unknown value!\n");
      break;
  }
}
