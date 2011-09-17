#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#ifdef __USE_PANTS_GC
#include <gc/gc.h>
#else
#define GC_MALLOC malloc
#define GC_INIT() 0
#endif

#define bool char
#define true 1
#define false 0
const unsigned int MAX_C_STRING_SIZE = 1024;
const unsigned int MIN_ARRAY_SIZE = 10;
const unsigned int MAX_ARRAY_ADDITION = 4096;
const char C_STRING_TRUNCATED_MESSAGE[] = "...";
void* EXTERNAL_FUNCTION_LABEL;

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
  void* frame;
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

struct Array {
  unsigned int size;
  unsigned int highwater;
  union Value* data;
};

typedef bool (*ExternalFunction)(void* environment,
    struct Array* right_positional_args, struct Array* left_positional_args,
    union Value* dest);

static inline union Value make_external_closure(ExternalFunction func,
    void* environment) {
  union Value v;
  v.t = CLOSURE;
  v.closure.func = EXTERNAL_FUNCTION_LABEL;
  v.closure.env = environment;
  v.closure.frame = func;
  return v;
}

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

static inline union Value make_c_string(char* format, ...) {
  va_list args;
  union Value str;
  unsigned int i;
  static unsigned int truncated_message_size = 0;
  str.t = STRING;
  str.string.byte_oriented = false;
  str.string.value = GC_MALLOC(MAX_C_STRING_SIZE);
  va_start(args, format);
  str.string.value_size = vsnprintf(str.string.value, MAX_C_STRING_SIZE, format,
      args);
  va_end(args);
  if(str.string.value_size >= MAX_C_STRING_SIZE) {
    if(truncated_message_size == 0)
      truncated_message_size = strlen(C_STRING_TRUNCATED_MESSAGE);
    str.string.value_size = MAX_C_STRING_SIZE - 1;
    for(i = 0; i < truncated_message_size; ++i) {
      str.string.value[MAX_C_STRING_SIZE - truncated_message_size + i]
          = C_STRING_TRUNCATED_MESSAGE[i];
    }
  }
  return str;
}

static inline union Value make_byte_string(char* data, unsigned int size) {
  union Value str;
  str.t = STRING;
  str.string.byte_oriented = true;
  str.string.value = data;
  str.string.value_size = size;
  return str;
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
      printf("string: %s\n", val.string.value);
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
