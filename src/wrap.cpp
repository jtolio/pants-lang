#include "wrap.h"

using namespace cirth::ir;

void cirth::wrap::ir_prepend(std::vector<PTR<Expression> >& ir) {

#define BIND_NAME_VAL(name, val) \
  ir.push_back(PTR<Expression>(new Definition(name, PTR<Value>( \
      new Variable(val)))));
#define BIND_SYMBOL(user_name, c_name) \
  BIND_NAME_VAL(Name(user_name, true), Name(c_name, false))
#define BIND_NAME(name) BIND_SYMBOL(name, name)

//  BIND_NAME_VAL(DICT_CONSTRUCTOR, Name("Dictionary", false));
//  BIND_NAME_VAL(ARRAY_CONSTRUCTOR, Name("Array", false));
  BIND_NAME("if");
//  BIND_NAME("while");
//  BIND_NAME("for");
  BIND_NAME("print");
//  BIND_NAME("construct");
//  BIND_NAME("import");
//  BIND_NAME("and");
//  BIND_NAME("or");
//  BIND_NAME("not");
  BIND_NAME("true");
  BIND_NAME("false");
//  BIND_NAME("assert");
//  BIND_NAME("in");
//  BIND_SYMBOL("==", "equals");
//  BIND_SYMBOL("<", "lessthan");
//  BIND_SYMBOL("<=", "lessthanequals");
//  BIND_SYMBOL(">", "greaterthan");
//  BIND_SYMBOL(">=", "greaterthanequals");
//  BIND_SYMBOL("!=", "notequals");
//  BIND_SYMBOL("+", "add");
//  BIND_SYMBOL("-", "subtract");
//  BIND_SYMBOL("/", "divide");
//  BIND_SYMBOL("*", "multiply");
  BIND_NAME_VAL(Name("null", true), NULL_VALUE);
//  BIND_NAME_VAL(THROW, EXCEPTION);
  BIND_NAME_VAL(RETURN, CONTINUATION);
//  BIND_NAME_VAL(Name("exit", true), CONTINUATION);

#undef BIND_NAME
#undef BIND_NAME_SYMBOL
#undef BIND_NAME_VAL

}

void cirth::wrap::remove_provided_names(std::set<Name>& names) {
#define REMOVE_NAME(name) names.erase(Name(name, false));

//  REMOVE_NAME("Dictionary");
//  REMOVE_NAME("Array");
  REMOVE_NAME("if");
//  REMOVE_NAME("while");
//  REMOVE_NAME("for");
  REMOVE_NAME("print");
//  REMOVE_NAME("construct");
//  REMOVE_NAME("import");
//  REMOVE_NAME("add");
//  REMOVE_NAME("subtract");
//  REMOVE_NAME("divide");
//  REMOVE_NAME("multiply");
//  REMOVE_NAME("and");
//  REMOVE_NAME("or");
//  REMOVE_NAME("not");
  REMOVE_NAME("true");
  REMOVE_NAME("false");
//  REMOVE_NAME("assert");
//  REMOVE_NAME("in");
//  REMOVE_NAME("equals");
//  REMOVE_NAME("lessthan");
//  REMOVE_NAME("lessthanequals");
//  REMOVE_NAME("greaterthan");
//  REMOVE_NAME("greaterthanequals");
//  REMOVE_NAME("notequals");

#undef REMOVE_NAME

  names.erase(NULL_VALUE);
//  names.erase(EXCEPTION);
  names.erase(CONTINUATION);
}
