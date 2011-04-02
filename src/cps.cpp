#include "cps.h"

using namespace cirth;

static std::string indent(unsigned int indent_level) {
  std::ostringstream os;
  for(unsigned int i = 0; i < indent_level; ++i) {
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
  os << "ObjectMutation(" << object->format(indent_level+1) << ", "
     << field.format(indent_level+1) << ", " << value->format(indent_level+1)
     << ",\n" << indent(indent_level)
     << next_expression->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::VariableMutation::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "VariableMutation(" << assignee.format(indent_level+1) << ", "
     << value->format(indent_level+1) << ",\n" << indent(indent_level)
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
  os << "Continuation(" << rv.format(indent_level+1) << ",\n"
     << indent(indent_level) << expression->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Scope::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Scope(\n" << indent(indent_level)
     << expression->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::PositionalOutArgument::format(unsigned int indent_level) const{
  std::ostringstream os;
  os << "PositionalOutArgument(" << variable->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::OptionalOutArgument::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "OptionalOutArgument(" << key.format(indent_level+1) << ", "
     << variable->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::ArbitraryOutArgument::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "ArbitraryOutArgument(" << variable->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::KeywordOutArgument::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "KeywordOutArgument(" << variable->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::PositionalInArgument::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "PositionalInArgument(" << variable.format(indent_level+1) << ")";
  return os.str();
}

std::string cps::OptionalInArgument::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "OptionalInArgument(" << variable.format(indent_level+1) << ", "
     << defaultval->format(indent_level+1) << ")";
  return os.str();
}

std::string cps::ArbitraryInArgument::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "ArbitraryInArgument(" << variable.format(indent_level+1) << ")";
  return os.str();
}

std::string cps::KeywordInArgument::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "KeywordInArgument(" << variable.format(indent_level+1) << ")";
  return os.str();
}

std::string cps::Array::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Array(\n" << indent(indent_level);
  for(unsigned int i = 0; i < values.size(); ++i) {
    if(i > 0) os << ",\n" << indent(indent_level);
    os << values[i]->format(indent_level+1);
  }
  os << ")";
  return os.str();
}

std::string cps::Dictionary::format(unsigned int indent_level) const {
  std::ostringstream os;
  os << "Dictionary(\n" << indent(indent_level);
  for(unsigned int i = 0; i < definitions.size(); ++i) {
    if(i > 0) os << ",\n" << indent(indent_level);
    os << "Definition(" << definitions[i].key->format(indent_level+2) << ", "
       << definitions[i].value->format(indent_level+2) << ")";
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
  os << "Field(" << object->format(indent_level+1) << ", "
     << field.format(indent_level+1) << ")";
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
    PTR<cps::Value> obj;
    ValueTranslation visitor(&obj);
    val->object->accept(&visitor);
    *rv = PTR<cps::Value>(new cps::Field(obj, val->field));
  }
  void visit(ir::Dictionary* old_dict) {
    PTR<cps::Dictionary> new_dict(new cps::Dictionary);
    *rv = new_dict;
    new_dict->definitions.reserve(old_dict->definitions.size());
    for(unsigned int i = 0; i < old_dict->definitions.size(); ++i) {
      PTR<cps::Value> key;
      PTR<cps::Value> val;
      ValueTranslation keyvisitor(&key);
      ValueTranslation valvisitor(&val);
      old_dict->definitions[i].key->accept(&keyvisitor);
      old_dict->definitions[i].value->accept(&valvisitor);
      new_dict->definitions.push_back(cps::DictDefinition(key, val));
    }
  }
  void visit(ir::Array* old_array) {
    PTR<cps::Array> new_array(new cps::Array);
    *rv = new_array;
    new_array->values.reserve(old_array->values.size());
    for(unsigned int i = 0; i < old_array->values.size(); ++i) {
      PTR<cps::Value> val;
      ValueTranslation visitor(&val);
      old_array->values[i]->accept(&visitor);
      new_array->values.push_back(val);
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
    call->right_positional_args.push_back(cps::PositionalOutArgument(
        PTR<cps::Value>(new cps::Variable(cps::Name("continuation", false,
        false)))));
    PTR<cps::Continuation> continuation(new cps::Continuation(cps::Name(
        "return", true, false)));
    continuation->expression = new_func->expression;
    call->callable = continuation;
    new_func->expression = call;
    // transform args
    new_func->left_positional_args.reserve(
        old_func->left_positional_args.size());
    for(unsigned int i = 0; i < old_func->left_positional_args.size(); ++i) {
      new_func->left_positional_args.push_back(cps::PositionalInArgument(
          old_func->left_positional_args[i].variable));
    }
    if(!!old_func->left_arbitrary_arg) {
      new_func->left_arbitrary_arg = cps::ArbitraryInArgument(
          old_func->left_arbitrary_arg.get().variable);
    }
    new_func->right_positional_args.reserve(
        old_func->right_positional_args.size());
    for(unsigned int i = 0; i < old_func->right_positional_args.size(); ++i) {
      new_func->right_positional_args.push_back(cps::PositionalInArgument(
          old_func->right_positional_args[i].variable));
    }
    new_func->right_optional_args.reserve(
        old_func->right_optional_args.size());
    for(unsigned int i = 0; i < old_func->right_optional_args.size(); ++i) {
      PTR<cps::Value> val;
      ValueTranslation visitor(&val);
      old_func->right_optional_args[i].defaultval->accept(&visitor);
      new_func->right_optional_args.push_back(cps::OptionalInArgument(
          old_func->right_optional_args[i].variable, val));
    }
    if(!!old_func->right_arbitrary_arg) {
      new_func->right_arbitrary_arg = cps::ArbitraryInArgument(
          old_func->right_arbitrary_arg.get().variable);
    }
    if(!!old_func->right_keyword_arg) {
      new_func->right_keyword_arg = cps::KeywordInArgument(
          old_func->right_keyword_arg.get().variable);
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
    call->right_positional_args.push_back(cps::PositionalOutArgument(trans(
        definition->value)));
    PTR<cps::Continuation> continuation(new cps::Continuation(
        definition->assignee));
    continuation->expression = *out_ir;
    call->callable = continuation;
    *out_ir = call;
  }
  void visit(ir::VariableMutation* varmutation) {
    *out_ir = PTR<cps::VariableMutation>(new cps::VariableMutation(
        varmutation->assignee, trans(varmutation->value), *out_ir));
  }
  void visit(ir::ObjectMutation* objmutation) {
    *out_ir = PTR<cps::ObjectMutation>(new cps::ObjectMutation(
        trans(objmutation->object), objmutation->field,
        trans(objmutation->value), *out_ir));
  }
  void visit(ir::ReturnValue* rv) {
    PTR<cps::Call> call(new cps::Call);
    call->callable = trans(rv->term->callable);
    call->left_positional_args.reserve(rv->term->left_positional_args.size());
    for(unsigned int i = 0; i < rv->term->left_positional_args.size(); ++i) {
      call->left_positional_args.push_back(cps::PositionalOutArgument(trans(
          rv->term->left_positional_args[i].variable)));
    }
    if(!!rv->term->left_arbitrary_arg) {
      call->left_arbitrary_arg = cps::ArbitraryOutArgument(trans(
          rv->term->left_arbitrary_arg.get().variable));
    }
    call->right_positional_args.reserve(
        rv->term->right_positional_args.size());
    for(unsigned int i = 0; i < rv->term->right_positional_args.size(); ++i) {
      call->right_positional_args.push_back(cps::PositionalOutArgument(trans(
          rv->term->right_positional_args[i].variable)));
    }
    call->right_optional_args.reserve(rv->term->right_optional_args.size());
    for(unsigned int i = 0; i < rv->term->right_optional_args.size(); ++i) {
      call->right_optional_args.push_back(cps::OptionalOutArgument(
          rv->term->right_optional_args[i].key,
          trans(rv->term->right_optional_args[i].variable)));
    }
    if(!!rv->term->right_arbitrary_arg) {
      call->right_arbitrary_arg = cps::ArbitraryOutArgument(trans(
          rv->term->right_arbitrary_arg.get().variable));
    }
    if(!!rv->term->right_keyword_arg) {
      call->right_keyword_arg = cps::KeywordOutArgument(trans(
          rv->term->right_keyword_arg.get().variable));
    }
    call->scoped_optional_args.reserve(rv->term->scoped_optional_args.size());
    for(unsigned int i = 0; i < rv->term->scoped_optional_args.size(); ++i) {
      call->scoped_optional_args.push_back(cps::OptionalOutArgument(
          rv->term->scoped_optional_args[i].key,
          trans(rv->term->scoped_optional_args[i].variable)));
    }
    if(!!rv->term->scoped_keyword_arg) {
      call->scoped_keyword_arg = cps::KeywordOutArgument(trans(
          rv->term->scoped_keyword_arg.get().variable));
    }
    PTR<cps::Continuation> continuation(new cps::Continuation(rv->assignee));
    continuation->expression = *out_ir;
    call->continuation = continuation;
    *out_ir = call;
  }
private:
  PTR<cps::Expression>* out_ir;
};

void cps::transform(const std::vector<PTR<ir::Expression> >& in_ir,
    const PTR<ir::Value>& in_lastval, PTR<cps::Expression>& out_ir) {
  PTR<cps::Call> call(new cps::Call);
  call->callable = PTR<cps::Value>(new cps::Variable(cps::Name("continuation",
      false, false)));
  call->right_positional_args.push_back(cps::PositionalOutArgument(trans(
      in_lastval)));
  out_ir = call;
  ExpressionTranslation visitor(&out_ir);
  for(unsigned int i = in_ir.size(); i > 0; --i) {
    in_ir[i-1]->accept(&visitor);
  }
}
