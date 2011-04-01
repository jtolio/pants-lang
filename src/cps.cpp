#include "cps.h"

using namespace cirth;

std::string cps::Call::format() const {
  std::ostringstream os;
  os << "Call(" << callable->format() << ", Left(";
  bool comma_needed = false;
  for(unsigned int i = 0; i < left_positional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << left_positional_args[i].format();
    comma_needed = true;
  }
  if(!!left_arbitrary_arg) {
    if(comma_needed) os << ", ";
    os << left_arbitrary_arg.get().format();
  }
  os << "), Right(";
  comma_needed = false;
  for(unsigned int i = 0; i < right_positional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << right_positional_args[i].format();
    comma_needed = true;
  }
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << right_optional_args[i].format();
    comma_needed = true;
  }
  if(!!right_arbitrary_arg) {
    if(comma_needed) os << ", ";
    os << right_arbitrary_arg.get().format();
    comma_needed = true;
  }
  if(!!right_keyword_arg) {
    if(comma_needed) os << ", ";
    os << right_keyword_arg.get().format();
  }
  os << "), Scoped(";
  comma_needed = false;
  for(unsigned int i = 0; i < scoped_optional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << scoped_optional_args[i].format();
    comma_needed = true;
  }
  if(!!scoped_keyword_arg) {
    if(comma_needed) os << ", ";
    os << scoped_keyword_arg.get().format();
  }
  os << ")";
  if(continuation.get())
    os << ", Continuation(" << continuation->format() << ")";
  if(exception.get())
    os << ", Exception(" << exception->format() << ")";
  os << ")";
  return os.str();
}

std::string cps::ObjectMutation::format() const {
  std::ostringstream os;
  os << "ObjectMutation(" << object->format() << ", " << field.format() << ", "
     << value->format() << ", " << next_expression->format() << ")";
  return os.str();
}

std::string cps::VariableMutation::format() const {
  std::ostringstream os;
  os << "VariableMutation(" << assignee.format() << ", " << value->format()
     << ", " << next_expression->format() << ")";
  return os.str();
}

std::string cps::Variable::format() const {
  std::ostringstream os;
  os << "Variable(" << variable.format() << ")";
  return os.str();
}

std::string cps::Function::format() const {
  std::ostringstream os;
  os << "Function(Left(";
  bool comma_needed = false;
  for(unsigned int i = 0; i < left_positional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << left_positional_args[i].format();
    comma_needed = true;
  }
  if(!!left_arbitrary_arg) {
    if(comma_needed) os << ", ";
    os << left_arbitrary_arg.get().format();
  }
  os << "), Right(";
  comma_needed = false;
  for(unsigned int i = 0; i < right_positional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << right_positional_args[i].format();
    comma_needed = true;
  }
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << right_optional_args[i].format();
    comma_needed = true;
  }
  if(!!right_arbitrary_arg) {
    if(comma_needed) os << ", ";
    os << right_arbitrary_arg.get().format();
    comma_needed = true;
  }
  if(!!right_keyword_arg) {
    if(comma_needed) os << ", ";
    os << right_keyword_arg.get().format();
  }
  os << "), " << expression->format() << ")";
  return os.str();
}

std::string cps::Continuation::format() const {
  std::ostringstream os;
  os << "Continuation(" << rv.format() << ", " << expression->format() << ")";
  return os.str();
}

std::string cps::Scope::format() const {
  std::ostringstream os;
  os << "Scope(" << expression->format() << ")";
  return os.str();
}

std::string cps::PositionalOutArgument::format() const {
  std::ostringstream os;
  os << "PositionalOutArgument(" << variable->format() << ")";
  return os.str();
}

std::string cps::OptionalOutArgument::format() const {
  std::ostringstream os;
  os << "OptionalOutArgument(" << key.format() << ", " << variable->format()
     << ")";
  return os.str();
}

std::string cps::ArbitraryOutArgument::format() const {
  std::ostringstream os;
  os << "ArbitraryOutArgument(" << variable->format() << ")";
  return os.str();
}

std::string cps::KeywordOutArgument::format() const {
  std::ostringstream os;
  os << "KeywordOutArgument(" << variable->format() << ")";
  return os.str();
}

std::string cps::PositionalInArgument::format() const {
  std::ostringstream os;
  os << "PositionalInArgument(" << variable.format() << ")";
  return os.str();
}

std::string cps::OptionalInArgument::format() const {
  std::ostringstream os;
  os << "OptionalInArgument(" << variable.format() << ", "
     << defaultval->format() << ")";
  return os.str();
}

std::string cps::ArbitraryInArgument::format() const {
  std::ostringstream os;
  os << "ArbitraryInArgument(" << variable.format() << ")";
  return os.str();
}

std::string cps::KeywordInArgument::format() const {
  std::ostringstream os;
  os << "KeywordInArgument(" << variable.format() << ")";
  return os.str();
}

static PTR<cps::Value> trans(ir::Field* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::Variable* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::Integer* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::CharString* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::ByteString* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::Float* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::Dictionary* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::Array* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::Function* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(ir::Scope* val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Value> trans(const PTR<ir::Value>& val) {
  return trans(val.get());
}

void cps::transform(const std::vector<PTR<ir::Expression> >& in_ir,
    const PTR<ir::Value>& in_lastval, PTR<cps::Expression>& out_ir) {
  PTR<cps::Call> call(new cps::Call);
  call->callable = PTR<cps::Value>(new cps::Variable(cps::Name("halt", false,
      false)));
  call->right_positional_args.push_back(cps::PositionalOutArgument(trans(
      in_lastval)));
  out_ir = call;

  for(unsigned int i = in_ir.size(); i > 0; --i) {
    ir::VariableMutation* varmutation(dynamic_cast<ir::VariableMutation*>(
        in_ir[i-1].get()));
    if(varmutation) {
      out_ir = PTR<cps::VariableMutation>(new cps::VariableMutation(
          varmutation->assignee, trans(varmutation->value), out_ir));
      continue;
    }
    ir::ObjectMutation* objmutation(dynamic_cast<ir::ObjectMutation*>(
        in_ir[i-1].get()));
    if(objmutation) {
      out_ir = PTR<cps::ObjectMutation>(new cps::ObjectMutation(
          trans(objmutation->object), objmutation->field,
          trans(objmutation->value), out_ir));
      continue;
    }
    ir::ReturnValue* rv(dynamic_cast<ir::ReturnValue*>(in_ir[i-1].get()));
    if(rv) {
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
      continuation->expression = out_ir;
      call->continuation = continuation;
      out_ir = call;
      continue;
    }
    ir::Definition* definition(dynamic_cast<ir::Definition*>(in_ir[i-1].get()));
    if(definition) {
      PTR<cps::Call> call(new cps::Call);
      call->right_positional_args.push_back(cps::PositionalOutArgument(trans(
          definition->value)));
      PTR<cps::Continuation> continuation(new cps::Continuation(
          definition->assignee));
      continuation->expression = out_ir;
      call->callable = continuation;
      out_ir = call;
      continue;
    }
   throw expectation_failure("unknown IR assignment type!");
  }
}
