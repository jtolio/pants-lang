#include "cps.h"

using namespace cirth;

static std::string indent(unsigned int indent_level) {
  std::ostringstream os;
  for(unsigned int i = 0; i <= indent_level; ++i) {
    os << "  ";
  }
  return os.str();
}

std::string cps::Call::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Call(\n" << indent(indent_level) << callable->format(indent_level+1)
     << ",\n" << indent(indent_level) << "Left(\n" << indent(indent_level+1);
  bool comma_needed = false;
  for(unsigned int i = 0; i < left_positional_args.size(); ++i) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << left_positional_args[i].format(indent_level+2);
    comma_needed = true;
  }
  if(!!left_arbitrary_arg) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << left_arbitrary_arg.get().format(indent_level+2);
  }
  os << "),\n" << indent(indent_level) << "Right(\n" << indent(indent_level+1);
  comma_needed = false;
  for(unsigned int i = 0; i < right_positional_args.size(); ++i) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << right_positional_args[i]->format(indent_level+2);
    comma_needed = true;
  }
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << right_optional_args[i].format(indent_level+2);
    comma_needed = true;
  }
  if(!!right_arbitrary_arg) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << right_arbitrary_arg.get().format(indent_level+2);
    comma_needed = true;
  }
  if(!!right_keyword_arg) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << right_keyword_arg.get().format(indent_level+2);
  }
  os << "),\n" << indent(indent_level) << "Scoped(\n" << indent(indent_level+1);
  comma_needed = false;
  for(unsigned int i = 0; i < scoped_optional_args.size(); ++i) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << scoped_optional_args[i].format(indent_level+2);
    comma_needed = true;
  }
  if(!!scoped_keyword_arg) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << scoped_keyword_arg.get().format(indent_level+2);
  }
  os << ")";
  if(continuation.get())
    os << ",\n" << indent(indent_level) << "Cont("
       << continuation->format(indent_level+1) << ")";
  if(exception.get())
    os << ",\n" << indent(indent_level) << "Exc("
       << exception->format(indent_level+1) << ")";
  os << ")";
  return os.str();
}

std::string cps::ObjectMutation::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "ObjectMutation(" << object.format(indent_level+1) << ", "
     << field.format(indent_level+1) << ", " << value.format(indent_level+1)
     << ",\n" << indent(indent_level)
     << next_expression->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::VariableMutation::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "VariableMutation(" << assignee.format(indent_level+1) << ", "
     << value.format(indent_level+1) << ",\n" << indent(indent_level)
     << next_expression->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Variable::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Variable(" << variable.format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Function::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Function(\n" << indent(indent_level) << "Left(\n"
     << indent(indent_level+1);
  bool comma_needed = false;
  for(unsigned int i = 0; i < left_positional_args.size(); ++i) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << left_positional_args[i].format(indent_level+2);
    comma_needed = true;
  }
  if(!!left_arbitrary_arg) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << left_arbitrary_arg.get().format(indent_level+2);
  }
  os << "),\n" << indent(indent_level) << "Right(\n" << indent(indent_level+1);
  comma_needed = false;
  for(unsigned int i = 0; i < right_positional_args.size(); ++i) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);;
    os << right_positional_args[i].format(indent_level+2);
    comma_needed = true;
  }
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);;
    os << right_optional_args[i].format(indent_level+2);
    comma_needed = true;
  }
  if(!!right_arbitrary_arg) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);;
    os << right_arbitrary_arg.get().format(indent_level+2);
    comma_needed = true;
  }
  if(!!right_keyword_arg) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);;
    os << right_keyword_arg.get().format(indent_level+2);
  }
  os << "),\n" << indent(indent_level) << expression->format(indent_level+1)
     << ")";
  return os.str();
}

std::string cps::Continuation::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Continuation(\n" << indent(indent_level);
  for(unsigned int i = 0; i < vars.size(); ++i) {
    os << vars[i].format(indent_level+1) << ",\n" << indent(indent_level);
  }
  os << expression->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Scope::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Scope(\n" << indent(indent_level)
     << expression->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Array::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Array(\n" << indent(indent_level);
  for(unsigned int i = 0; i < values.size(); ++i) {
    if(i > 0) os << ",\n" << indent(indent_level);
    os << values[i].format(indent_level+1);
  }
  os << ")";
  return os.str();
}

std::string cps::Dictionary::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Dictionary(\n" << indent(indent_level);
  for(unsigned int i = 0; i < definitions.size(); ++i) {
    if(i > 0) os << ",\n" << indent(indent_level);
    os << definitions[i].format(indent_level+1);
  }
  os << ")";
  return os.str();
}

std::string cps::Float::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Float(" << value << ")";
  return os.str();
}

std::string cps::ByteString::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "ByteString(" << value << ")";
  return os.str();
}

std::string cps::CharString::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "CharString(" << value << ")";
  return os.str();
}

std::string cps::Integer::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Integer(" << value << ")";
  return os.str();
}

std::string cps::Field::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Field(" << object.format(indent_level+1) << ", "
     << field.format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Definition::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Definition(" << key.format(indent_level+1) << ", "
     << value.format(indent_level+1) << ")";
  return os.str();
}

class ValueTranslation : public ir::ValueVisitor {
public:
  ValueTranslation(PTR<cps::Value>* rv_) : rv(rv_) {}
  void visit(ir::Variable* var) {
    *rv = PTR<cps::Value>(new cps::Variable(var->variable));
  }
  void visit(ir::Integer* val) {
    *rv = PTR<cps::Value>(new cps::Integer(val->value));
  }
  void visit(ir::CharString* val) {
    *rv = PTR<cps::Value>(new cps::CharString(val->value));
  }
  void visit(ir::ByteString* val) {
    *rv = PTR<cps::Value>(new cps::ByteString(val->value));
  }
  void visit(ir::Float* val) {
    *rv = PTR<cps::Value>(new cps::Float(val->value));
  }
  void visit(ir::Field* val) {
    *rv = PTR<cps::Value>(new cps::Field(val->object, val->field));
  }
  void visit(ir::Dictionary* old_dict) {
    PTR<cps::Dictionary> new_dict(new cps::Dictionary);
    *rv = new_dict;
    new_dict->definitions.reserve(old_dict->definitions.size());
    for(unsigned int i = 0; i < old_dict->definitions.size(); ++i) {
      new_dict->definitions.push_back(cps::Definition(
          old_dict->definitions[i].key, old_dict->definitions[i].value));
    }
  }
  void visit(ir::Array* old_array) {
    PTR<cps::Array> new_array(new cps::Array);
    *rv = new_array;
    new_array->values.reserve(old_array->values.size());
    for(unsigned int i = 0; i < old_array->values.size(); ++i) {
      new_array->values.push_back(old_array->values[i]);
    }
  }
  void visit(ir::Scope* old_scope) {
    PTR<cps::Scope> new_scope(new cps::Scope);
    *rv = new_scope;
    cps::transform(old_scope->expressions, old_scope->lastval,
        new_scope->expression);
  }
  void visit(ir::Function* old_func) {
    PTR<cps::Function> new_func(new cps::Function);
    *rv = new_func;
    // transform internals
    cps::transform(old_func->expressions, old_func->lastval,
        new_func->expression);
    // redefine return
    PTR<cps::Call> call(new cps::Call);
    call->right_positional_args.push_back(PTR<cps::Value>(
        new cps::Variable(cps::Name("continuation", false, false))));
    PTR<cps::Continuation> continuation(new cps::Continuation);
    continuation->vars.push_back(cps::Name("return", true, false));
    continuation->expression = new_func->expression;
    call->callable = continuation;
    new_func->expression = call;
    // transform args
    new_func->left_positional_args.reserve(
        old_func->left_positional_args.size());
    for(unsigned int i = 0; i < old_func->left_positional_args.size(); ++i) {
      new_func->left_positional_args.push_back(
          old_func->left_positional_args[i].variable);
    }
    if(!!old_func->left_arbitrary_arg) {
      new_func->left_arbitrary_arg =
          old_func->left_arbitrary_arg.get().variable;
    }
    new_func->right_positional_args.reserve(
        old_func->right_positional_args.size());
    for(unsigned int i = 0; i < old_func->right_positional_args.size(); ++i) {
      new_func->right_positional_args.push_back(
          old_func->right_positional_args[i].variable);
    }
    new_func->right_optional_args.reserve(
        old_func->right_optional_args.size());
    for(unsigned int i = 0; i < old_func->right_optional_args.size(); ++i) {
      new_func->right_optional_args.push_back(cps::Definition(
          old_func->right_optional_args[i].variable,
          old_func->right_optional_args[i].defaultval));
    }
    if(!!old_func->right_arbitrary_arg) {
      new_func->right_arbitrary_arg =
          old_func->right_arbitrary_arg.get().variable;
    }
    if(!!old_func->right_keyword_arg) {
      new_func->right_keyword_arg =
          old_func->right_keyword_arg.get().variable;
    }
  }
private:
  PTR<cps::Value>* rv;
};

static PTR<cps::Value> trans(const PTR<ir::Value>& val) {
  PTR<cps::Value> rv;
  ValueTranslation visitor(&rv);
  val->accept(&visitor);
  return rv;
}

class ExpressionTranslation : public ir::ExpressionVisitor {
public:
  ExpressionTranslation(PTR<cps::Expression>* out_ir_) : out_ir(out_ir_) {}
  void visit(ir::Definition* definition) {
    PTR<cps::Call> call(new cps::Call);
    call->right_positional_args.push_back(trans(definition->value));
    PTR<cps::Continuation> continuation(new cps::Continuation);
    continuation->vars.push_back(definition->assignee);
    continuation->expression = *out_ir;
    call->callable = continuation;
    *out_ir = call;
  }
  void visit(ir::VariableMutation* varmutation) {
    *out_ir = PTR<cps::VariableMutation>(new cps::VariableMutation(
        varmutation->assignee, varmutation->value, *out_ir));
  }
  void visit(ir::ObjectMutation* objmutation) {
    *out_ir = PTR<cps::ObjectMutation>(new cps::ObjectMutation(
        objmutation->object, objmutation->field, objmutation->value, *out_ir));
  }
  void visit(ir::ReturnValue* rv) {
    PTR<cps::Call> call(new cps::Call);
    call->callable = PTR<cps::Value>(new cps::Variable(rv->term->callable));
    call->left_positional_args.reserve(rv->term->left_positional_args.size());
    for(unsigned int i = 0; i < rv->term->left_positional_args.size(); ++i) {
      call->left_positional_args.push_back(
          rv->term->left_positional_args[i].variable);
    }
    if(!!rv->term->left_arbitrary_arg) {
      call->left_arbitrary_arg =
          rv->term->left_arbitrary_arg.get().variable;
    }
    call->right_positional_args.reserve(
        rv->term->right_positional_args.size());
    for(unsigned int i = 0; i < rv->term->right_positional_args.size(); ++i) {
      call->right_positional_args.push_back(
          PTR<cps::Value>(new cps::Variable(
          rv->term->right_positional_args[i].variable)));
    }
    call->right_optional_args.reserve(rv->term->right_optional_args.size());
    for(unsigned int i = 0; i < rv->term->right_optional_args.size(); ++i) {
      call->right_optional_args.push_back(cps::Definition(
          rv->term->right_optional_args[i].key,
          rv->term->right_optional_args[i].variable));
    }
    if(!!rv->term->right_arbitrary_arg) {
      call->right_arbitrary_arg =
          rv->term->right_arbitrary_arg.get().variable;
    }
    if(!!rv->term->right_keyword_arg) {
      call->right_keyword_arg =
          rv->term->right_keyword_arg.get().variable;
    }
    call->scoped_optional_args.reserve(rv->term->scoped_optional_args.size());
    for(unsigned int i = 0; i < rv->term->scoped_optional_args.size(); ++i) {
      call->scoped_optional_args.push_back(cps::Definition(
          rv->term->scoped_optional_args[i].key,
          rv->term->scoped_optional_args[i].variable));
    }
    if(!!rv->term->scoped_keyword_arg) {
      call->scoped_keyword_arg =
          rv->term->scoped_keyword_arg.get().variable;
    }
    PTR<cps::Continuation> continuation(new cps::Continuation);
    continuation->vars.push_back(rv->assignee);
    continuation->expression = *out_ir;
    call->continuation = continuation;
    *out_ir = call;
  }
private:
  PTR<cps::Expression>* out_ir;
};

void cps::transform(const std::vector<PTR<ir::Expression> >& in_ir,
    const ir::Name& in_lastval, PTR<cps::Expression>& out_ir) {
  PTR<cps::Call> call(new cps::Call);
  call->callable = PTR<cps::Value>(new cps::Variable(cps::Name("continuation",
      false, false)));
  call->right_positional_args.push_back(PTR<cps::Value>(new cps::Variable(
      in_lastval)));
  out_ir = call;
  ExpressionTranslation visitor(&out_ir);
  for(unsigned int i = in_ir.size(); i > 0; --i) {
    in_ir[i-1]->accept(&visitor);
  }
}

static void callables_in_values(PTR<cps::Value> value,
    std::vector<PTR<cps::Callable> >& callables) {
  if(!value) return;
  PTR<cps::Callable> callable =
      boost::dynamic_pointer_cast<cps::Callable>(value);
  if(!callable) return;
  callables.push_back(callable);
  callable->expression->callables(callables);
}

static void names_in_values(PTR<cps::Value> value,
    std::vector<cps::Name>& names) {
  if(!value) return;
//  check for field, variable, callable, dictionary, array,
}

void cps::Call::callables(std::vector<PTR<cps::Callable> >& callables) {
  callables_in_values(callable, callables);
  for(unsigned int i = 0; i < right_positional_args.size(); ++i) {
    callables_in_values(right_positional_args[i], callables);
  }
  callables_in_values(continuation, callables);
  callables_in_values(exception, callables);
}

void cps::Call::free_names(std::set<Name>& names) {
  names_in_values(callable, names);
  for(unsigned int i = 0; i < left_positional_args.size(); ++i)
    names.erase(left_positional_args[i]);
  if(!!left_arbitrary_arg) names.erase(left_arbitrary_arg.get());
  for(unsigned int i = 0; i < right_positional_args.size(); ++i)
    names.erase(right_positional_args[i]);
  for(unsigned int i = 0; i < right_optional_args.size(); ++i)
    names.erase(right_optional_args[i].key);
  if(!!right_arbitrary_arg) names.erase(right_arbitrary_arg.get());
  if(!!right_keyword_arg) names.erase(right_keyword_arg.get());
//  for(unsigned int i = 0; i < scoped_optional_args.size(); ++i)
//    names.erase(scoped_optional_args[i].key);
//  if(!!scoped_keyword_arg) names.erase(scoped_keyword_arg.get());
  for(unsigned int i = 0; i < right_optional_args.size(); ++i)
    names.insert(right_optional_args[i].value);
  names_in_values(continuation);
  names_in_values(exception);
}
