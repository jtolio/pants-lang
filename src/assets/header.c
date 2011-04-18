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
  union Value c_lessthan;
  union Value c_equals;
  union Value c_add;
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
    case STRING:
    case OBJECT:
    case CLOSURE:
      return true;
    default:
      printf("unimplemented!\n");
      exit(1);
  }
}

static inline union Value make_cell(union Value val) {
  union Value v;
  v.t = CELL;
  v.cell.addr = GC_MALLOC(sizeof(union Value));
  *(v.cell.addr) = val;
  return v;
}

static inline void print_value(union Value val) {
  switch(val.t) {
    case INTEGER:
      printf("%lld", val.integer.value);
      break;
    case FLOAT:
      printf("%f", val.floating.value);
      break;
    case BOOLEAN:
      if(val.boolean.value) {
        printf("true");
      } else {
        printf("false");
      }
      break;
    case NIL:
      printf("null");
      break;

    case CLOSURE:
    case STRING:
    case OBJECT:
    default:
      printf("<TODO: unimplemented>");
      break;
  }
}

static inline bool builtin_less_than(union Value val1, union Value val2) {
  switch(val1.t) {
    case INTEGER:
      switch(val2.t) {
        case INTEGER: return val1.integer.value < val2.integer.value;
        case FLOAT: return val1.integer.value < val2.floating.value;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case FLOAT:
      switch(val2.t) {
        case INTEGER: return val1.floating.value < val2.integer.value;
        case FLOAT: return val1.floating.value < val2.floating.value;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case BOOLEAN:
      switch(val2.t) {
        case INTEGER: return true;
        case FLOAT: return true;
        case BOOLEAN: return val1.boolean.value < val2.boolean.value;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case NIL:
      switch(val2.t) {
        case INTEGER: return true;
        case FLOAT: return true;
        case BOOLEAN: return true;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case CLOSURE:
      switch(val2.t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE:
          if(val1.closure.func < val2.closure.func) return true;
          if(val1.closure.func != val2.closure.func) return false;
          return val1.closure.env < val2.closure.env;
        case STRING: return false;
        case OBJECT: return false;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case STRING:
      switch(val2.t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING:
          printf("TODO: unimplemented!\n");
          exit(1);
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case OBJECT:
      switch(val2.t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return false;
        case OBJECT:
          printf("TODO: unimplemented!\n");
          exit(1);
        default:
          printf("unknown type!\n");
          exit(1);
      }
    default:
      printf("unknown type!\n");
      exit(1);
  }
  return false;
}

static inline bool builtin_equals(union Value val1, union Value val2) {
  if(!((val1.t == INTEGER && val2.t == FLOAT) ||
       (val1.t == FLOAT && val2.t == INTEGER)) &&
     val1.t != val2.t)
    return false;
  switch(val1.t) {
    case INTEGER:
      if(val2.t == INTEGER) {
        return val1.integer.value == val2.integer.value;
      } else {
        return val1.integer.value == val2.floating.value;
      }
    case FLOAT:
      if(val2.t == INTEGER) {
        return val1.floating.value == val2.integer.value;
      } else {
        return val1.floating.value == val2.floating.value;
      }
    case BOOLEAN:
      return val1.boolean.value == val2.boolean.value;
    case NIL:
      return true;
    case CLOSURE:
      return val1.closure.func == val2.closure.func &&
          val1.closure.env == val2.closure.env;

    case STRING:
    case OBJECT:
    default:
      printf("TODO: unimplemented\n");
      exit(1);
      return false;
  }
}

static inline union Value builtin_add(union Value val1, union Value val2) {
  union Value rv;
  rv.t = NIL;
  switch(val1.t) {
    case INTEGER:
      switch(val2.t) {
        case INTEGER:
          rv.t = INTEGER;
          rv.integer.value = val1.integer.value + val2.integer.value;
          return rv;
        case FLOAT:
          rv.t = FLOAT;
          rv.floating.value = val1.integer.value + val2.floating.value;
          return rv;
        case STRING:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    case FLOAT:
      switch(val2.t) {
        case INTEGER:
          rv.t = FLOAT;
          rv.floating.value = val1.floating.value + val2.integer.value;
          return rv;
        case FLOAT:
          rv.t = FLOAT;
          rv.floating.value = val1.floating.value + val2.floating.value;
          return rv;
        case STRING:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    case STRING:
      printf("TODO: unimplemented\n");
      exit(1);
      return rv;
    case OBJECT:
      printf("TODO: unimplemented\n");
      exit(1);
      return rv;
    case BOOLEAN:
      switch(val2.t) {
        case STRING:
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    case NIL:
      switch(val2.t) {
        case STRING:
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    case CLOSURE:
      switch(val2.t) {
        case STRING:
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    default:
      printf("unsupported addition!\n");
      exit(1);
      return rv;
  }
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
