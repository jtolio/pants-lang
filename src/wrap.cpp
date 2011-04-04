#include "wrap.h"

using namespace cirth::ir;

void cirth::wrap::ir_prepend(std::vector<PTR<Expression> >& ir) {

#define BIND_NAME_VAL(name, val) \
  ir.push_back(PTR<Expression>(new Definition(name, PTR<Value>( \
      new Variable(val)))));
#define BIND_NAME(name) BIND_NAME_VAL(Name(name, true), Name(name, false))

  BIND_NAME_VAL(DICT_CONSTRUCTOR, Name("Dictionary", false));
  BIND_NAME_VAL(ARRAY_CONSTRUCTOR, Name("Array", false));
  BIND_NAME("if");
  BIND_NAME("while");
  BIND_NAME("for");
  BIND_NAME("print");
  BIND_NAME("construct");
  BIND_NAME("import");
  BIND_NAME("+");
  BIND_NAME("-");
  BIND_NAME("/");
  BIND_NAME("*");
  BIND_NAME("and");
  BIND_NAME("or");
  BIND_NAME("not");
  BIND_NAME("true");
  BIND_NAME("false");
  BIND_NAME("assert");
  BIND_NAME("in");
  BIND_NAME("==");
  BIND_NAME("<");
  BIND_NAME("<=");
  BIND_NAME(">");
  BIND_NAME(">=");
  BIND_NAME("!=");
  BIND_NAME_VAL(Name("null", true), NULL_VALUE);
  BIND_NAME_VAL(THROW, EXCEPTION);
  BIND_NAME_VAL(RETURN, CONTINUATION);
  BIND_NAME_VAL(Name("exit", true), CONTINUATION);

#undef BIND_NAME
#undef BIND_NAME_VAL

}
