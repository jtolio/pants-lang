#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define HEADER \
  "#include <stdlib.h>\n#include <stdio.h>\n\n#ifdef DO_GC\n#include <gc/gc.h>\n#else\n#define GC_MALLOC(size) malloc(size)\n#define GC_MALLOC_ATOMIC(size) malloc(size)\n#define GC_INIT() {}\n#endif\n\n#define bool char\n#define true 1\n#define false 0\n\nenum Tag {\n    INTEGER,\n    FLOAT,\n    STRING,\n    OBJECT,\n    BOOLEAN,\n    NIL,\n    CLOSURE\n  };\n\nunion Value;\n\nstruct Integer {\n  enum Tag t;\n  long long value;\n};\n\nstruct Float {\n  enum Tag t;\n  double value;\n};\n\nstruct String {\n  enum Tag t;\n  char* value;\n  bool byte_oriented;\n};\n\nstruct Object {\n  enum Tag t;\n};\n\nstruct Boolean {\n  enum Tag t;\n  bool value;\n};\n\nstruct Nil {\n  enum Tag t;\n};\n\nstruct Sentinel {\n  enum Tag t;\n};\n\nstruct Closure {\n  enum Tag t;\n  void* func;\n  void* env;\n};\n\nunion Value {\n  enum Tag t;\n  struct Integer integer;\n  struct Float floating;\n  struct String string;\n  struct Object object;\n  struct Boolean boolean;\n  struct Closure closure;\n};\n\nstruct env_main {\n  union Value c_continuation;\n  union Value c_null;\n  union Value c_print;\n};\n"

#define STARTMAIN \
  "int main(int argc, char **argv) {\n" \
  "\tstruct env_main main;\n" \
  "\tmain.c_continuation.t = CLOSURE;\n" \
  "\tmain.c_continuation.closure.func = &&halt;\n" \
  "\tmain.c_continuation.closure.env = NULL;\n" \
  "\tmain.c_null.t = NIL;\n" \
  "\tmain.c_print.t = CLOSURE;\n" \
  "\tmain.c_print.closure.func = &&c_print;\n" \
  "\tmain.c_print.closure.env = NULL;\n" \
  "\n" \
  "\tvoid* env = &main;\n" \
  "\tunion Value dest;\n" \
  "\tGC_INIT();\n" \
  "\n" \
  "\tunion Value* right_positional_args = NULL;\n" \
  "\tunsigned int right_positional_args_size = 0;\n" \
  "\tbool has_continuation = false;\n" \
  "\tunion Value continuation;\n" \
  "\n" \
  "\tgoto start;\n" \
  "\n" \
  "c_print:\n" \
  "\tprintf(\"print!\\n\");\n" \
  "\tenv = continuation.closure.env;\n" \
  "\tgoto *continuation.closure.func;\n" \
  "\n" \
  "start:\n"

#endif
