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
  BIND_NAME("new_object");
  BIND_NAME("seal_object");
  BIND_NAME("if");
  BIND_NAME("print");
//  BIND_NAME("construct");
//  BIND_NAME("import");
  BIND_NAME("true");
  BIND_NAME("false");
  BIND_SYMBOL("==", "equals");
  BIND_SYMBOL("<", "lessthan");
  BIND_SYMBOL("+", "add");
//  BIND_SYMBOL("-", "subtract");
//  BIND_SYMBOL("/", "divide");
//  BIND_SYMBOL("*", "multiply");
  BIND_NAME_VAL(Name("null", true), NULL_VALUE);
  BIND_NAME_VAL(RETURN, CONTINUATION);
  BIND_NAME_VAL(Name("exit", true), CONTINUATION);

#undef BIND_NAME
#undef BIND_NAME_SYMBOL
#undef BIND_NAME_VAL

}

void cirth::wrap::remove_provided_names(std::set<Name>& names) {
#define REMOVE_NAME(name) names.erase(Name(name, false));

//  REMOVE_NAME("Dictionary");
//  REMOVE_NAME("Array");
  REMOVE_NAME("new_object");
  REMOVE_NAME("seal_object");
  REMOVE_NAME("if");
  REMOVE_NAME("print");
//  REMOVE_NAME("construct");
//  REMOVE_NAME("import");
  REMOVE_NAME("add");
//  REMOVE_NAME("subtract");
//  REMOVE_NAME("divide");
//  REMOVE_NAME("multiply");
  REMOVE_NAME("true");
  REMOVE_NAME("false");
  REMOVE_NAME("equals");
  REMOVE_NAME("lessthan");

#undef REMOVE_NAME

  names.erase(NULL_VALUE);
  names.erase(CONTINUATION);
  names.erase(HIDDEN_OBJECT);
}
