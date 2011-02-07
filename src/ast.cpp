#include "ast.h"

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
  os << "FullValue(" << value->format();
  for(unsigned int i = 0; i < trailers.size(); ++i) {
    os << ", " << trailers[i]->format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ast::Variable::format() const {
  std::ostringstream os;
  os << "Variable(" << name << ")";
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

std::string cirth::ast::Call::format() const { return "Call()"; }

std::string cirth::ast::Lookup::format() const {
  std::ostringstream os;
  os << "Lookup(" << variable.format() << ")";
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

