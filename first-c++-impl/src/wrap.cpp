#include "wrap.h"

using namespace pants::ir;

void pants::wrap::ir_prepend(std::vector<PTR<Expression> >& ir) {

#define BIND_NAME_VAL(name, val) \
  ir.push_back(PTR<Expression>(new Assignment(name, PTR<Value>( \
      new Variable(val)), true)));
#define BIND_SYMBOL(user_name, c_name) \
  BIND_NAME_VAL(Name(user_name, true), Name(c_name, false))
#define BIND_NAME(name) BIND_SYMBOL(name, name)

//  BIND_NAME_VAL(DICT_CONSTRUCTOR, Name("Dictionary", false));
  BIND_NAME_VAL(ARRAY_CONSTRUCTOR, Name("Array", false));
  BIND_NAME("DynamicVar");
  BIND_NAME("Integer");
  BIND_NAME("Float");
  BIND_NAME("ByteString");
  BIND_NAME("CharString");
  BIND_NAME("Boolean");
  BIND_NAME("Null");
  BIND_NAME("Function");
  BIND_NAME("throw_dynamic_var");
  BIND_NAME("new_object");
  BIND_NAME("seal_object");
  BIND_NAME("if");
  BIND_NAME("register_main");
  BIND_NAME("print");
  BIND_NAME("type");
  BIND_NAME("println");
  BIND_NAME("readln");
//  BIND_NAME("construct");
//  BIND_NAME("import");
  BIND_NAME("true");
  BIND_NAME("false");
  BIND_SYMBOL("==", "equals");
  BIND_SYMBOL("<", "lessthan");
  BIND_SYMBOL("+", "add");
  BIND_SYMBOL("-", "subtract");
  BIND_SYMBOL("/", "divide");
  BIND_SYMBOL("*", "multiply");
  BIND_SYMBOL("%", "modulo");
  BIND_NAME_VAL(Name("null", true), NULL_VALUE);
  BIND_NAME_VAL(RETURN, CONTINUATION);
  BIND_NAME_VAL(Name("exit", true), CONTINUATION);

#undef BIND_NAME
#undef BIND_NAME_SYMBOL
#undef BIND_NAME_VAL

}

void pants::wrap::provided_names(std::set<Name>& names) {
#define ADD_NAME(name) names.insert(Name(name, false));

//  ADD_NAME("Dictionary");
  ADD_NAME("Array");
  ADD_NAME("DynamicVar");
  ADD_NAME("throw_dynamic_var");
  ADD_NAME("new_object");
  ADD_NAME("seal_object");
  ADD_NAME("if");
  ADD_NAME("register_main");
  ADD_NAME("print");
  ADD_NAME("println");
  ADD_NAME("readln");
//  ADD_NAME("construct");
//  ADD_NAME("import");
  ADD_NAME("add");
  ADD_NAME("subtract");
  ADD_NAME("divide");
  ADD_NAME("multiply");
  ADD_NAME("modulo");
  ADD_NAME("true");
  ADD_NAME("false");
  ADD_NAME("equals");
  ADD_NAME("lessthan");
  ADD_NAME("type");
  ADD_NAME("Integer");
  ADD_NAME("Float");
  ADD_NAME("ByteString");
  ADD_NAME("CharString");
  ADD_NAME("Boolean");
  ADD_NAME("Null");
  ADD_NAME("Function");

#undef ADD_NAME

  names.insert(NULL_VALUE);
  names.insert(CONTINUATION);
  names.insert(DYNAMIC_VARS);
}
