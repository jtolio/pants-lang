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
    FUNCTION,
    CONTINUATION,
    SCOPE
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

struct Function {
  enum Tag t;
};

struct Continuation {
  enum Tag t;
};

struct Scope {
  enum Tag t;
};

union Value {
  enum Tag t;
  struct Integer integer;
  struct Float floating;
  struct String string;
  struct Object object;
  struct Boolean boolean;
  struct Function function;
  struct Continuation continuation;
  struct Scope scope;
};

static union Value MakeInteger(long long n) {
  static union Value v;
  v.t = INTEGER;
  v.integer.value = n;
  return v;
}

static union Value MakeFloat(double val) {
  static union Value v;
  v.t = FLOAT;
  v.floating.value = val;
  return v;
}

static union Value MakeString(char* val, bool byte_oriented) {
  static union Value v;
  v.t = STRING;
  v.string.value = val;
  v.string.byte_oriented = byte_oriented;
  return v;
}

static union Value MakeObject() {
  static union Value v;
  v.t = OBJECT;
  return v;
}

static union Value MakeBoolean(bool val) {
  static union Value v;
  v.t = BOOLEAN;
  v.boolean.value = val;
  return v;
}

static union Value MakeNull() {
  static union Value v;
  v.t = NIL;
  return v;
}

static union Value MakeFunction() {
  static union Value v;
  v.t = FUNCTION;
  return v;
}

static union Value MakeContinuation() {
  static union Value v;
  v.t = CONTINUATION;
  return v;


static union Value MakeScope() {
  static union Value v;
  v.t = SCOPE;
  return v;
}

static inline bool isTrue(union Value v) {
  switch(v.t) {
    case INTEGER: return v.integer.value != 0;
    case FLOAT: return v.floating.value != 0;
    case BOOLEAN: return v.boolean.value;
    case NIL: return false;
    default: return true;
  }
}
