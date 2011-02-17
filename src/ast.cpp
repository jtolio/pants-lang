#include "ast.h"

std::string cirth::ast::Application::format() const {
  std::ostringstream os;
  os << "Application(";
  for(unsigned int i = 0; i < terms.size(); ++i) {
    if(i > 0) os << ", ";
    os << terms[i]->format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ast::Assignment::format() const {
  std::ostringstream os;
  os << name() << "(" << assignee->format() << ", " << exp->format() << ")";
  return os.str();
}

std::string cirth::ast::Term::format() const {
  std::ostringstream os;
  os << "Term(Headers(";
  for(unsigned int i = 0; i < headers.size(); ++i) {
    if(i > 0) os << ", ";
    os << headers[i]->format();
  }
  os << "), Value(" << value->format() << "), Trailers(";
  for(unsigned int i = 0; i < trailers.size(); ++i) {
    if(i > 0) os << ", ";
    os << trailers[i]->format();
  }
  os << "))";
  return os.str();
}

std::string cirth::ast::Variable::format() const {
  std::ostringstream os;
  os << "Variable(" << name << (user_provided ? ", user_provided)" :
      ", compiler_provided)");
  return os.str();
}

std::string cirth::ast::SubExpression::format() const {
  std::ostringstream os;
  os << "SubExpression(";
  for(unsigned int i = 0; i < expressions.size(); ++i) {
    if(i > 0) os << ", ";
    os << expressions[i]->format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ast::Integer::format() const {
  std::ostringstream os;
  os << "Integer(" << value << ")";
  return os.str();
}

std::string cirth::ast::CharString::format() const {
  std::ostringstream os;
  os << "CharString(" << value << ")";
  return os.str();
}

std::string cirth::ast::ByteString::format() const {
  std::ostringstream os;
  os << "ByteString(" << value << ")";
  return os.str();
}

std::string cirth::ast::Float::format() const {
  std::ostringstream os;
  os << "Float(" << value << ")";
  return os.str();
}

std::string cirth::ast::DictDefinition::format() const {
  std::ostringstream os;
  os << "DictDefinition(" << key->format() << ", " << value->format() << ")";
  return os.str();
}

cirth::ast::Dictionary::Dictionary(const std::vector<DictDefinition>& values_) {
  values.reserve(values_.size());
  for(unsigned int i = 0; i < values_.size(); ++i) {
    if(values_[i].key.get() && values_[i].value.get())
      values.push_back(values_[i]);
  }
}

std::string cirth::ast::Dictionary::format() const {
  std::ostringstream os;
  os << "Dictionary(";
  for(unsigned int i = 0; i < values.size(); ++i) {
    if(i > 0) os << ", ";
    os << values[i].format();
  }
  os << ")";
  return os.str();
}

cirth::ast::Array::Array(const std::vector<PTR<Expression> >& values_) {
  values.reserve(values_.size());
  for(unsigned int i = 0; i < values_.size(); ++i) {
    if(values_[i].get()) values.push_back(values_[i]);
  }
}

std::string cirth::ast::Array::format() const {
  std::ostringstream os;
  os << "Array(";
  for(unsigned int i = 0; i < values.size(); ++i) {
    if(i > 0) os << ", ";
    os << values[i]->format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ast::RequiredInArgument::format() const {
  std::ostringstream os;
  os << "RequiredInArgument(" << name.format() << ")";
  return os.str();
}

std::string cirth::ast::OptionalInArgument::format() const {
  std::ostringstream os;
  os << "OptionalInArgument(" << name.format() << ", " << application->format()
     << ")";
  return os.str();
}

std::string cirth::ast::ArbitraryInArgument::format() const {
  std::ostringstream os;
  os << "ArbitraryInArgument(" << name.format() << ")";
  return os.str();
}

std::string cirth::ast::KeywordInArgument::format() const {
  std::ostringstream os;
  os << "KeywordInArgument(" << name.format() << ")";
  return os.str();
}

std::string cirth::ast::RequiredOutArgument::format() const {
  std::ostringstream os;
  os << "RequiredOutArgument(" << application->format() << ")";
  return os.str();
}

std::string cirth::ast::OptionalOutArgument::format() const {
  std::ostringstream os;
  os << "OptionalOutArgument(" << name.format() << ", " << application->format()
     << ")";
  return os.str();
}

std::string cirth::ast::ArbitraryOutArgument::format() const {
  std::ostringstream os;
  os << "ArbitraryOutArgument(";
  for(unsigned int i = 0; i < array.size(); ++i) {
    if(i > 0) os << ", ";
    os << array[i]->format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ast::KeywordOutArgument::format() const {
  std::ostringstream os;
  os << "KeywordOutArgument(";
  for(unsigned int i = 0; i < object.size(); ++i) {
    if(i > 0) os << ", ";
    os << object[i]->format();
  }
  os << ")";
  return os.str();
}

static void loadHalfArgs(
    const std::vector<PTR<cirth::ast::InArgument> >& in_args,
    cirth::ast::HalfArgs& arg_store) {
  using namespace cirth::ast;
  arg_store.required_args.reserve(in_args.size());
  arg_store.optional_args.reserve(in_args.size());
  for(unsigned int i = 0; i < in_args.size(); ++i) {
    if(!in_args[i].get()) continue;
    if(dynamic_cast<ArbitraryInArgument*>(in_args[i].get())) {
      if(!!arg_store.arbitrary_arg)
        throw expectation_failure(
            "only one arbitrary argument expected");
      arg_store.arbitrary_arg = *((ArbitraryInArgument*)in_args[i].get());
    } else if(dynamic_cast<KeywordInArgument*>(in_args[i].get())) {
      if(!!arg_store.keyword_arg)
        throw expectation_failure("only one keyword argument expected");
      arg_store.keyword_arg = *((KeywordInArgument*)in_args[i].get());
    } else if(dynamic_cast<OptionalInArgument*>(in_args[i].get())) {
      arg_store.optional_args.push_back(
          *((OptionalInArgument*)in_args[i].get()));
    } else if(dynamic_cast<RequiredInArgument*>(in_args[i].get())) {
      arg_store.required_args.push_back(
          *((RequiredInArgument*)in_args[i].get()));
    } else {
      throw expectation_failure("unknown argument type");
    }
  }
}

cirth::ast::Function::Function(const boost::optional<InArgList>& args_,
    const std::vector<PTR<Expression> >& expressions_)
  : expressions(expressions_)
{
  if(!args_) return;

  if(!!args_->left_args)
    loadHalfArgs(args_->left_args.get(), left);

  loadHalfArgs(args_->right_args, right);
}

std::string cirth::ast::HalfArgs::format() const {
  std::ostringstream os;
  os << "Required(";
  for(unsigned int i = 0; i < required_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << required_args[i].format();
  }
  os << "), Optional(";
  for(unsigned int i = 0; i < optional_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << optional_args[i].format();
  }
  os << ")";
  if(!!arbitrary_arg)
    os << ", Arbitrary(" << arbitrary_arg->name.format() << ")";
  if(!!keyword_arg)
    os << ", Keyword(" << keyword_arg->name.format() << ")";
  return os.str();
}

std::string cirth::ast::Function::format() const {
  std::ostringstream os;
  os << "Function(Left(" << left.format() << "), Right(" << right.format()
     << "), Expressions(";
  for(unsigned int i = 0; i < expressions.size(); ++i) {
    if(i > 0) os << ", ";
    os << expressions[i]->format();
  }
  os << "))";
  return os.str();
}

std::string cirth::ast::OpenCall::format() const { return "OpenCall()"; }

std::string cirth::ast::Field::format() const {
  std::ostringstream os;
  os << "Field(" << variable.format() << ")";
  return os.str();
}

std::string cirth::ast::Index::format() const {
  std::ostringstream os;
  os << "Index(";
  for(unsigned int i = 0; i < expressions.size(); ++i) {
    if(i > 0) os << ", ";
    os << expressions[i]->format();
  }
  os << ")";
  return os.str();
}

cirth::ast::ClosedCall::ClosedCall(
    const boost::optional<std::vector<PTR<OutArgument> > >& left_args_,
    const std::vector<PTR<OutArgument> >& right_args_,
    const boost::optional<std::vector<PTR<OutArgument> > >& scoped_args_) {
  if(!!left_args_) {
    left_args.reserve(left_args_.get().size());
    for(unsigned int i = 0; i < left_args_.get().size(); ++i) {
      if(left_args_.get()[i].get()) left_args.push_back(left_args_.get()[i]);
    }
  }
  right_args.reserve(right_args_.size());
  for(unsigned int i = 0; i < right_args_.size(); ++i) {
    if(right_args_[i].get()) right_args.push_back(right_args_[i]);
  }
  if(!!scoped_args_) {
    scoped_args.reserve(scoped_args_.get().size());
    for(unsigned int i = 0; i < scoped_args_.get().size(); ++i) {
      if(scoped_args_.get()[i].get())
        scoped_args.push_back(scoped_args_.get()[i]);
    }
  }
}

std::string cirth::ast::ClosedCall::format() const {
  std::ostringstream os;
  os << "ClosedCall(Left(";
  for(unsigned int i = 0; i < left_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << left_args[i]->format();
  }
  os << "), Right(";
  for(unsigned int i = 0; i < right_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << right_args[i]->format();
  }
  os << "), Scoped(";
  for(unsigned int i = 0; i < scoped_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << scoped_args[i]->format();
  }
  os << "))";
  return os.str();
}

std::string cirth::ast::Assignee::format() const {
  std::ostringstream os;
  os << "Assignee(" << term->format() << ")";
  return os.str();
}
