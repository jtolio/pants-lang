#include "cps.h"

using namespace cirth;

unsigned int cps::Callable::m_varcount = 0;

static std::string indent(unsigned int indent_level) {
  std::ostringstream os;
  for(unsigned int i = 0; i <= indent_level; ++i) {
    os << "  ";
  }
  return os.str();
}

std::string cps::Call::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Call(\n" << indent(indent_level) << "Left(\n"
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
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << right_positional_args[i].format(indent_level+2);
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
  os << "),\n" << indent(indent_level) << "HiddenObject(\n"
     << indent(indent_level+1);
  comma_needed = false;
  for(unsigned int i = 0; i < hidden_object_optional_args.size(); ++i) {
    if(comma_needed) os << ",\n" << indent(indent_level+1);
    os << hidden_object_optional_args[i].format(indent_level+2);
    comma_needed = true;
  }
  os << ")";
  if(continuation.get())
    os << ",\n" << indent(indent_level) << "Cont("
       << continuation->format(indent_level+1) << ")";
  os << ",\n" << indent(indent_level) << callable.format(indent_level+1)
     << ")";
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

std::string cps::Assignment::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Assignment(" << (local ? "local, " : "nonlocal, ")
     << assignee.format(indent_level+1) << ", "
     << value->format(indent_level+1) << ",\n" << indent(indent_level)
     << next_expression->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Variable::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Variable(" << variable.format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Callable::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Callable(" << (function ? "function,\n" : "continuation,\n")
     << indent(indent_level) << "Left(\n" << indent(indent_level+1);
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

std::string cps::Float::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Float(" << value << ")";
  return os.str();
}

std::string cps::String::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "String(" << value << (byte_oriented ?
      ", byte_oriented)" : ", char_oriented)");
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
    *rv = PTR<cps::Value>(new cps::String(val->value, false));
  }
  void visit(ir::ByteString* val) {
    *rv = PTR<cps::Value>(new cps::String(val->value, true));
  }
  void visit(ir::Float* val) {
    *rv = PTR<cps::Value>(new cps::Float(val->value));
  }
  void visit(ir::Field* val) {
    *rv = PTR<cps::Value>(new cps::Field(val->object, val->field));
  }
  void visit(ir::Function* old_func) {
    PTR<cps::Callable> new_func(new cps::Callable(true));
    *rv = new_func;
    cps::transform(old_func->expressions, old_func->lastval,
        new_func->expression);
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
  void visit(ir::Assignment* assignment) {
    *out_ir = PTR<cps::Assignment>(new cps::Assignment(assignment->assignee,
        trans(assignment->value), assignment->local, *out_ir));
  }
  void visit(ir::ObjectMutation* objmutation) {
    *out_ir = PTR<cps::ObjectMutation>(new cps::ObjectMutation(
        objmutation->object, objmutation->field, objmutation->value, *out_ir));
  }
  void visit(ir::ReturnValue* rv) {
    PTR<cps::Call> call(new cps::Call(rv->term->callable));
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
          rv->term->right_positional_args[i].variable);
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
    call->hidden_object_optional_args.reserve(
        rv->term->hidden_object_optional_args.size());
    for(unsigned int i = 0; i < rv->term->hidden_object_optional_args.size();
        ++i) {
      call->hidden_object_optional_args.push_back(cps::Definition(
          rv->term->hidden_object_optional_args[i].key,
          rv->term->hidden_object_optional_args[i].variable));
    }
    PTR<cps::Callable> continuation(new cps::Callable(false));
    continuation->right_positional_args.push_back(rv->assignee);
    continuation->expression = *out_ir;
    call->continuation = continuation;
    *out_ir = call;
  }
private:
  PTR<cps::Expression>* out_ir;
};

void cps::transform(const std::vector<PTR<ir::Expression> >& in_ir,
    const ir::Name& in_lastval, PTR<cps::Expression>& out_ir) {
  PTR<cps::Call> call(new cps::Call(CONTINUATION));
  call->right_positional_args.push_back(in_lastval);
  out_ir = call;
  ExpressionTranslation visitor(&out_ir);
  for(unsigned int i = in_ir.size(); i > 0; --i) {
    in_ir[i-1]->accept(&visitor);
  }
}

static void callables_in_values(PTR<cps::Value> value,
    std::vector<PTR<cps::Callable> >& callables) {
  if(!value) return;
  PTR<cps::Callable> callable(
      boost::dynamic_pointer_cast<cps::Callable>(value));
  if(!callable) return;
  callables.push_back(callable);
  callable->expression->callables(callables);
}

static void free_names_in_values(PTR<cps::Value> value,
    std::set<cps::Name>& names) {
  if(!value) return;
  value->free_names(names);
}

void cps::Call::callables(std::vector<PTR<cps::Callable> >&
    callables) {
  callables_in_values(continuation, callables);
}

void cps::Assignment::callables(
    std::vector<PTR<cps::Callable> >& callables) {
  next_expression->callables(callables);
  callables_in_values(value, callables);
}

void cps::Call::free_names(std::set<Name>& names) {
  names.insert(callable);
  for(unsigned int i = 0; i < left_positional_args.size(); ++i)
    names.insert(left_positional_args[i]);
  if(!!left_arbitrary_arg) names.insert(left_arbitrary_arg.get());
  for(unsigned int i = 0; i < right_positional_args.size(); ++i)
    names.insert(right_positional_args[i]);
  for(unsigned int i = 0; i < right_optional_args.size(); ++i)
    names.insert(right_optional_args[i].value);
  for(unsigned int i = 0; i < hidden_object_optional_args.size(); ++i)
    names.insert(hidden_object_optional_args[i].value);
  if(!!right_arbitrary_arg) names.insert(right_arbitrary_arg.get());
  if(!!right_keyword_arg) names.insert(right_keyword_arg.get());
  free_names_in_values(continuation, names);
  names.insert(HIDDEN_OBJECT);
}

static inline void add_unique_name(std::set<cps::Name>& names,
    const cps::Name& name) {
  if(names.find(name) != names.end())
    throw expectation_failure("non-unique arg name");
  names.insert(name);
}

void cps::Callable::arg_names(std::set<cps::Name>& names) {
  std::set<cps::Name> args;
  for(unsigned int i = 0; i < left_positional_args.size(); ++i)
    add_unique_name(args, left_positional_args[i]);
  if(!!left_arbitrary_arg) add_unique_name(args, left_arbitrary_arg.get());
  for(unsigned int i = 0; i < right_positional_args.size(); ++i)
    add_unique_name(args, right_positional_args[i]);
  for(unsigned int i = 0; i < right_optional_args.size(); ++i)
    add_unique_name(args, right_optional_args[i].key);
  if(!!right_arbitrary_arg) add_unique_name(args, right_arbitrary_arg.get());
  if(!!right_keyword_arg) add_unique_name(args, right_keyword_arg.get());
  if(function) {
    add_unique_name(args, HIDDEN_OBJECT);
    add_unique_name(args, CONTINUATION);
  }
  for(std::set<Name>::iterator it(args.begin()); it != args.end(); ++it)
    names.insert(*it);
}

void cps::Callable::free_names(std::set<cps::Name>& names) {
  std::set<Name> new_names;
  expression->free_names(new_names);
  std::set<Name> args;
  arg_names(args);
  for(std::set<Name>::iterator it(args.begin()); it != args.end(); ++it)
    new_names.erase(*it);
  for(std::set<Name>::iterator it(new_names.begin());
      it != new_names.end(); ++it)
    names.insert(*it);
  for(unsigned int i = 0; i < right_optional_args.size(); ++i)
    names.insert(right_optional_args[i].value);
}

void cps::Assignment::free_names(std::set<cps::Name>& names) {
  if(!local) {
    next_expression->free_names(names);
    value->free_names(names);
    names.insert(assignee);
    return;
  }
  std::set<Name> new_names;
  next_expression->free_names(new_names);
  value->free_names(new_names);
  new_names.erase(assignee);
  for(std::set<Name>::iterator it(new_names.begin());
      it != new_names.end(); ++it)
    names.insert(*it);
}

void cps::Call::frame_names(std::set<Name>& names) {
  if(continuation) continuation->frame_names(names);
}
