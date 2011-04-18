#include <stdlib.h>
#include <stdio.h>
#include <gc/gc.h>

#define bool char
#define true 1
#define false 0

enum Tag {
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
  enum Tag t;
  long long value;
};

struct Float {
  enum Tag t;
  double value;
};

struct String {
  enum Tag t;
  char* value;
  bool byte_oriented;
};

struct Object {
  enum Tag t;
};

struct Boolean {
  enum Tag t;
  bool value;
};

struct Nil {
  enum Tag t;
};

struct Sentinel {
  enum Tag t;
};

struct Closure {
  enum Tag t;
  void* func;
  void* env;
};

struct Cell {
  enum Tag t;
  union Value* addr;
};

union Value {
  enum Tag t;
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
};

static inline bool is_true(union Value val) {
  switch(val.t) {
    case INTEGER:
    case FLOAT:
      return val.integer.value != 0;
    case BOOLEAN:
      return val.boolean.value;
    case NIL:
      return false;
    default:
      return true;
  }
}

static inline union Value make_cell(union Value val) {
  union Value v;
  v.t = CELL;
  v.cell.addr = GC_MALLOC(sizeof(union Value));
  *(v.cell.addr) = val;
  return v;
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
