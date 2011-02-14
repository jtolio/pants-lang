#include "ast.h"

cirth::ast::List::List(const std::vector<PTR<Expression> >& values_) {
  values.reserve(values_.size());
  for(unsigned int i = 0; i < values_.size(); ++i) {
    if(values_[i].get()) values.push_back(values_[i]);
  }
}

std::string cirth::ast::List::format() const {
  std::ostringstream os;
  os << "List(";
  for(unsigned int i = 0; i < values.size(); ++i) {
    if(i > 0) os << ", ";
    os << values[i]->format();
  }
  os << ")";
  return os.str();
}

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

std::string cirth::ast::ListExpansion::format() const {
  std::ostringstream os;
  os << "ListExpansion(";
  for(unsigned int i = 0; i < expressions.size(); ++i) {
    if(i > 0) os << ", ";
    os << expressions[i]->format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ast::FullValue::format() const {
  std::ostringstream os;
  os << "FullValue(Headers(";
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

std::string cirth::ast::MapDefinition::format() const {
  std::ostringstream os;
  os << "MapDefinition(" << key->format() << ", " << value->format() << ")";
  return os.str();
}

cirth::ast::Map::Map(const std::vector<MapDefinition>& values_) {
  values.reserve(values_.size());
  for(unsigned int i = 0; i < values_.size(); ++i) {
    if(values_[i].key.get()) values.push_back(values_[i]);
  }
}

std::string cirth::ast::Map::format() const {
  std::ostringstream os;
  os << "Map(";
  for(unsigned int i = 0; i < values.size(); ++i) {
    if(i > 0) os << ", ";
    os << values[i].format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ast::RequiredArgument::format() const {
  std::ostringstream os;
  os << "RequiredArgument(" << name.format() << ")";
  return os.str();
}
  
std::string cirth::ast::OptionalArgument::format() const {
  std::ostringstream os;
  os << "OptionalArgument(" << name.format() << ", " << value->format() << ")";
  return os.str();
}
  
std::string cirth::ast::ArbitraryArgument::format() const {
  std::ostringstream os;
  os << "ArbitraryArgument(" << name.format() << ")";
  return os.str();
}
  
std::string cirth::ast::KeywordArgument::format() const {
  std::ostringstream os;
  os << "KeywordArgument(" << name.format() << ")";
  return os.str();
}
  
cirth::ast::Function::Function(const boost::optional<ArgList>& args_,
    const std::vector<PTR<Expression> >& expressions_)
  : expressions(expressions_)
{
  if(!args_) return;

  if(!!args_->left_args) {
    const std::vector<PTR<Argument> > &left_args_ = args_->left_args.get();
    left_required_args.reserve(left_args_.size());
    left_optional_args.reserve(left_args_.size());
    for(unsigned int i = 0; i < left_args_.size(); ++i) {
      if(!left_args_[i].get()) continue;
      if(dynamic_cast<ArbitraryArgument*>(left_args_[i].get())) {
        if(!!left_arbitrary_arg)
          throw expectation_failure(
              "only one left arbitrary argument expected");
        left_arbitrary_arg = *((ArbitraryArgument*)left_args_[i].get());
      } else if(dynamic_cast<KeywordArgument*>(left_args_[i].get())) {
        if(!!left_keyword_arg)
          throw expectation_failure("only one left keyword argument expected");
        left_keyword_arg = *((KeywordArgument*)left_args_[i].get());
      } else if(dynamic_cast<OptionalArgument*>(left_args_[i].get())) {
        left_optional_args.push_back(*((OptionalArgument*)left_args_[i].get()));
      } else if(dynamic_cast<RequiredArgument*>(left_args_[i].get())) {
        left_required_args.push_back(*((RequiredArgument*)left_args_[i].get()));
      } else {
        throw expectation_failure("unknown argument type");
      }
    }
  }

  const std::vector<PTR<Argument> > &right_args_ = args_->right_args;
  right_required_args.reserve(right_args_.size());
  right_optional_args.reserve(right_args_.size());
  for(unsigned int i = 0; i < right_args_.size(); ++i) {
    if(!right_args_[i].get()) continue;
    if(dynamic_cast<RequiredArgument*>(right_args_[i].get())) {
      right_required_args.push_back(*((RequiredArgument*)right_args_[i].get()));
    } else if(dynamic_cast<OptionalArgument*>(right_args_[i].get())) {
      right_optional_args.push_back(*((OptionalArgument*)right_args_[i].get()));
    } else if(dynamic_cast<ArbitraryArgument*>(right_args_[i].get())) {
      if(!!right_arbitrary_arg)
        throw expectation_failure("only one right arbitrary argument expected");
      right_arbitrary_arg = *((ArbitraryArgument*)right_args_[i].get());
    } else if(dynamic_cast<KeywordArgument*>(right_args_[i].get())) {
      if(!!right_keyword_arg)
        throw expectation_failure("only one right keyword argument expected");
      right_keyword_arg = *((KeywordArgument*)right_args_[i].get());
    } else {
      throw expectation_failure("unknown argument type");
    }
  }
}

std::string cirth::ast::Function::format() const {
  std::ostringstream os;
  os << "Function(";
  os << "Left(Required(";
  for(unsigned int i = 0; i < left_required_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << left_required_args[i].format();
  }
  os << "), Optional(";
  for(unsigned int i = 0; i < left_optional_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << left_optional_args[i].format();
  }
  os << ")";
  if(!!left_arbitrary_arg)
    os << ", Arbitrary(" << left_arbitrary_arg->name.format() << ")";
  if(!!left_keyword_arg)
    os << ", Keyword(" << left_arbitrary_arg->name.format() << ")";
  os << "), Right(Required(";
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
    const std::vector<PTR<Expression> >& arguments_) {
  arguments.reserve(arguments_.size());
  for(unsigned int i = 0; i < arguments_.size(); ++i) {
    if(arguments_[i].get()) arguments.push_back(arguments_[i]);
  }
}

std::string cirth::ast::ClosedCall::format() const {
  std::ostringstream os;
  os << "ClosedCall(";
  for(unsigned int i = 0; i < arguments.size(); ++i) {
    if(i > 0) os << ", ";
    os << arguments[i]->format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ast::SingleAssignee::format() const {
  std::ostringstream os;
  os << "SingleAssignee(" << fullvalue->format() << ")";
  return os.str();
}

cirth::ast::AssigneeList::AssigneeList(
    const std::vector<PTR<Assignee> >& assignees_) {
  assignees.reserve(assignees_.size());
  for(unsigned int i = 0; i < assignees_.size(); ++i) {
    if(assignees_[i].get()) assignees.push_back(assignees_[i]);
  }
}

std::string cirth::ast::AssigneeList::format() const {
  std::ostringstream os;
  os << "AssigneeList(";
  for(unsigned int i = 0; i < assignees.size(); ++i) {
    if(i > 0) os << ", ";
    os << assignees[i]->format();
  }
  os << ")";
  return os.str();
}

