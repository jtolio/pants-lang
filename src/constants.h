#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define HEADER \
  "#include <stdlib.h>\n" \
  "#include <stdio.h>\n" \

#define STARTMAIN \
  "int main(int argc, char **argv) {\n" \
  "\tvoid* env = NULL;\n" \
  "\tunion Value dest;\n" \
  "\tdest.t = BOOLEAN;\n" \
  "\n" \
  "\tGC_INIT();\n"

#endif
