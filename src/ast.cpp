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

cirth::ast::Function::Function(const boost::optional<ArgList>& args,
    const std::vector<PTR<Expression> >& expressions_)
  : expressions(expressions_)
{
  if(!args) return;
  right_args = args->rightargs.args;
  if(!!args->rightargs.optional_args) {
    assert(!args->rightargs.var_arg);
    right_optional_args = args->rightargs.optional_args->optional_args;
    right_var_arg = args->rightargs.optional_args->var_arg;
  } else if(!!args->rightargs.var_arg) {
    right_var_arg = args->rightargs.var_arg;
  }
  if(!args->leftargs) return;
  left_args = args->leftargs->args;
  if(!!args->leftargs->optional_args) {
    assert(!args->leftargs->var_arg);
    left_optional_args = args->leftargs->optional_args->optional_args;
    left_var_arg = args->leftargs->optional_args->var_arg;
  } else if(!!args->leftargs->var_arg) {
    left_var_arg = args->leftargs->var_arg;
  }
}

std::string cirth::ast::Function::format() const {
  std::ostringstream os;
  os << "Function(";
  os << "LeftArgs(";
  for(unsigned int i = 0; i < left_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << left_args[i].format();
  }
  os << "), LeftOptionalArgs(";
  for(unsigned int i = 0; i < left_optional_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << left_optional_args[i].format();
  }
  os << "), ";
  if(!!left_var_arg)
    os << "LeftVariadicArg(" << left_var_arg->name.format() << ", " <<
        left_var_arg->subexpression->format() << "), ";
  os << "RightArgs(";
  for(unsigned int i = 0; i < right_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << right_args[i].format();
  }
  os << "), RightOptionalArgs(";
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(i > 0) os << ", ";
    os << right_optional_args[i].format();
  }
  os << "), ";
  if(!!right_var_arg)
    os << "RightVariadicArg(" << right_var_arg->name.format() << ", " <<
        right_var_arg->subexpression->format() << "), ";
  os << "Expressions(";
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

