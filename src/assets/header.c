#include <stdlib.h>
#include <stdio.h>

#ifdef DO_GC
#include <gc/gc.h>
#else
#define GC_MALLOC(size) malloc(size)
#define GC_MALLOC_ATOMIC(size) malloc(size)
#define GC_INIT() {}
#endif

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
  CLOSURE
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

union Value {
  enum Tag t;
  struct Integer integer;
  struct Float floating;
  struct String string;
  struct Object object;
  struct Boolean boolean;
  struct Closure closure;
};

struct env_main {
  union Value c_continuation;
  union Value c_null;
  union Value c_print;
};
