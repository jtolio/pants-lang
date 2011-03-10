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

cirth::ast::Term::Term(const std::vector<PTR<ValueModifier> >& headers_,
    PTR<Value> value_, const std::vector<PTR<ValueModifier> >& trailers_)
  : value(value_)
{
  trailers.reserve(trailers_.size() + headers_.size());
  for(unsigned int i = 0; i < trailers_.size(); ++i) {
    if(trailers_[i]) trailers.push_back(trailers_[i]);
  }
  for(unsigned int i = headers_.size(); i > 0; --i) {
    if(headers_[i-1]) trailers.push_back(headers_[i-1]);
  }
}

std::string cirth::ast::Term::format() const {
  std::ostringstream os;
  os << "Term(Value(" << value->format() << "), Trailers(";
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

cirth::ast::SubExpression::SubExpression(const std::vector<PTR<Expression> >&
    expressions_) {
  expressions.reserve(expressions_.size());
  for(unsigned int i = 0; i < expressions_.size(); ++i) {
    if(expressions_[i]) expressions.push_back(expressions_[i]);
  }
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

cirth::ast::ArbitraryOutArgument::ArbitraryOutArgument(
    const std::vector<PTR<Expression> >& array_) {
  array.reserve(array_.size());
  for(unsigned int i = 0; i < array_.size(); ++i) {
    if(array_[i]) array.push_back(array_[i]);
  }
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

cirth::ast::KeywordOutArgument::KeywordOutArgument(
    const std::vector<PTR<Expression> >& object_) {
  object.reserve(object_.size());
  for(unsigned int i = 0; i < object_.size(); ++i) {
    if(object_[i]) object.push_back(object_[i]);
  }
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

cirth::ast::Function::Function(const boost::optional<InArgList>& args_,
    const std::vector<PTR<Expression> >& expressions_)
{
  expressions.reserve(expressions_.size());
  for(unsigned int i = 0; i < expressions_.size(); ++i) {
    if(expressions_[i]) expressions.push_back(expressions_[i]);
  }
  
  if(!args_) return;

  if(!!args_->left_args) {
    const std::vector<PTR<InArgument> >& left_args(args_->left_args.get());
    left_required_args.reserve(left_args.size());
    for(unsigned int i = 0; i < left_args.size(); ++i) {
      if(!left_args[i].get()) continue;
      if(dynamic_cast<RequiredInArgument*>(left_args[i].get())) {
        left_required_args.push_back(
            *((RequiredInArgument*)left_args[i].get()));
      } else if(dynamic_cast<ArbitraryInArgument*>(left_args[i].get())) {
        throw cirth::expectation_failure("left arbitrary argument not "
            "supported");
      } else if(dynamic_cast<KeywordInArgument*>(left_args[i].get())) {
        throw cirth::expectation_failure("left keyword argument not "
            "supported");
      } else if(dynamic_cast<OptionalInArgument*>(left_args[i].get())) {
        throw cirth::expectation_failure("left optional argument not "
            "supported");
      } else {
        throw cirth::expectation_failure("unknown argument type");
      }
    }
  }

  const std::vector<PTR<InArgument> >& right_args(args_->right_args);
  right_required_args.reserve(right_args.size());
  right_optional_args.reserve(right_args.size());
  for(unsigned int i = 0; i < right_args.size(); ++i) {
    if(!right_args[i].get()) continue;
    if(dynamic_cast<ArbitraryInArgument*>(right_args[i].get())) {
      if(!!right_arbitrary_arg)
        throw cirth::expectation_failure(
            "only one arbitrary argument expected");
      right_arbitrary_arg = *((ArbitraryInArgument*)right_args[i].get());
    } else if(dynamic_cast<KeywordInArgument*>(right_args[i].get())) {
      if(!!right_keyword_arg)
        throw cirth::expectation_failure("only one keyword argument expected");
      right_keyword_arg = *((KeywordInArgument*)right_args[i].get());
    } else if(dynamic_cast<OptionalInArgument*>(right_args[i].get())) {
      right_optional_args.push_back(
          *((OptionalInArgument*)right_args[i].get()));
    } else if(dynamic_cast<RequiredInArgument*>(right_args[i].get())) {
      right_required_args.push_back(
          *((RequiredInArgument*)right_args[i].get()));
    } else {
      throw cirth::expectation_failure("unknown argument type");
    }
  }
}

std::string cirth::ast::Function::format() const {
  std::ostringstream os;
  os << "Function(Left(";
  os << "Required(";
  for(unsigned int i = 0; i < left_required_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << left_required_args[i].format();
  }
  os << ")), Right(";
  os << "Required(";
  for(unsigned int i = 0; i < right_required_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << right_required_args[i].format();
  }
  os << "), Optional(";
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << right_optional_args[i].format();
  }
  os << ")";
  if(!!right_arbitrary_arg)
    os << ", Arbitrary(" << right_arbitrary_arg->name.format() << ")";
  if(!!right_keyword_arg)
    os << ", Keyword(" << right_keyword_arg->name.format() << ")";
  os << "), Expressions(";
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

cirth::ast::Index::Index(
    const std::vector<PTR<Expression> >& expressions_) {
  expressions.reserve(expressions_.size());
  for(unsigned int i = 0; i < expressions_.size(); ++i) {
    if(expressions_[i]) expressions.push_back(expressions_[i]);
  }
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
    left_required_args.reserve(left_args_.get().size());
    for(unsigned int i = 0; i < left_args_.get().size(); ++i) {
      if(!left_args_.get()[i].get()) continue;
      if(dynamic_cast<RequiredOutArgument*>(left_args_.get()[i].get())) {
        left_required_args.push_back(
            *((RequiredOutArgument*)left_args_.get()[i].get()));
      } else if(dynamic_cast<ArbitraryOutArgument*>(left_args_.get()[i].get())) {
        throw cirth::expectation_failure("left arbitrary argument not "
            "supported");
      } else if(dynamic_cast<KeywordOutArgument*>(left_args_.get()[i].get())) {
        throw cirth::expectation_failure("left keyword argument not supported");
      } else if(dynamic_cast<OptionalOutArgument*>(left_args_.get()[i].get())) {
        throw cirth::expectation_failure("left optional argument not "
            "supported");
      } else {
        throw cirth::expectation_failure("unknown argument type");
      }
    }
  }
  
  right_required_args.reserve(right_args_.size());
  right_optional_args.reserve(right_args_.size());
  for(unsigned int i = 0; i < right_args_.size(); ++i) {
    if(!right_args_[i].get()) continue;
    if(dynamic_cast<ArbitraryOutArgument*>(right_args_[i].get())) {
      if(!!right_arbitrary_arg)
        throw cirth::expectation_failure(
            "only one arbitrary argument expected");
      right_arbitrary_arg = *((ArbitraryOutArgument*)right_args_[i].get());
    } else if(dynamic_cast<KeywordOutArgument*>(right_args_[i].get())) {
      if(!!right_keyword_arg)
        throw cirth::expectation_failure("only one keyword argument expected");
      right_keyword_arg = *((KeywordOutArgument*)right_args_[i].get());
    } else if(dynamic_cast<OptionalOutArgument*>(right_args_[i].get())) {
      right_optional_args.push_back(
          *((OptionalOutArgument*)right_args_[i].get()));
    } else if(dynamic_cast<RequiredOutArgument*>(right_args_[i].get())) {
      right_required_args.push_back(
          *((RequiredOutArgument*)right_args_[i].get()));
    } else {
      throw cirth::expectation_failure("unknown argument type");
    }
  }

  if(!!scoped_args_) {
    scoped_optional_args.reserve(scoped_args_.get().size());
    for(unsigned int i = 0; i < scoped_args_.get().size(); ++i) {
      if(!scoped_args_.get()[i].get()) continue;
      if(dynamic_cast<ArbitraryOutArgument*>(scoped_args_.get()[i].get())) {
        throw cirth::expectation_failure("scoped arbitrary argument not "
            "supported");
      } else if(dynamic_cast<KeywordOutArgument*>(scoped_args_.get()[i].get())){
        if(!!scoped_keyword_arg)
          throw cirth::expectation_failure("only one keyword argument "
              "expected");
        scoped_keyword_arg = *(
            (KeywordOutArgument*)scoped_args_.get()[i].get());
      } else if(dynamic_cast<OptionalOutArgument*>(
          scoped_args_.get()[i].get())) {
        scoped_optional_args.push_back(
            *((OptionalOutArgument*)scoped_args_.get()[i].get()));
      } else if(dynamic_cast<RequiredOutArgument*>(
          scoped_args_.get()[i].get())) {
        throw cirth::expectation_failure("scoped positional argument not "
            "supported");
      } else {
        throw cirth::expectation_failure("unknown argument type");
      }
    }
  }
}

std::string cirth::ast::ClosedCall::format() const {
  std::ostringstream os;
  os << "ClosedCall(Left(";
  for(unsigned int i = 0; i < left_required_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << left_required_args[i].format();
  }
  os << "), Right(";
  for(unsigned int i = 0; i < right_required_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << right_required_args[i].format();
  }
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(i > 0 || right_required_args.size() > 0) os << ", ";
    os << right_optional_args[i].format();
  }
  if(!!right_arbitrary_arg) {
    if(right_required_args.size() + right_optional_args.size() > 0) os << ", ";
    os << right_arbitrary_arg.get().format();
  }
  if(!!right_keyword_arg) {
    if(right_required_args.size() + right_optional_args.size() > 0 ||
        !!right_arbitrary_arg) os << ", ";
    os << right_keyword_arg.get().format();
  }
  os << "), Scoped(";
  for(unsigned int i = 0; i < scoped_optional_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << scoped_optional_args[i].format();
  }
  if(!!scoped_keyword_arg) {
    if(scoped_optional_args.size() > 0) os << ", ";
    os << scoped_keyword_arg.get().format();
  }
  os << "))";
  return os.str();
}

std::string cirth::ast::Assignee::format() const {
  std::ostringstream os;
  os << "Assignee(" << term->format() << ")";
  return os.str();
}
