#ifndef __CPS_TRANSFORM_H__
#define __CPS_TRANSFORM_H__

#include "common.h"
#include "ir.h"

namespace cirth {
namespace cps {

  typedef cirth::ir::Name Name;

  struct Value {
    virtual ~Value() {}
    virtual std::string format(unsigned int indent_level) const = 0;
    protected: Value() {} };

  struct Field : public Value {
    Field(const Name& object_, const Name& field_)
      : object(object_), field(field_) {}
    std::string format(unsigned int indent_level) const;
    Name object;
    Name field;
  };

  struct Variable : public Value {
    Variable(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format(unsigned int indent_level) const;
  };

  struct Integer : public Value {
    Integer(const long long& value_) : value(value_) {}
    long long value;
    std::string format(unsigned int indent_level) const;
  };

  struct CharString : public Value {
    CharString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format(unsigned int indent_level) const;
  };

  struct ByteString : public Value {
    ByteString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format(unsigned int indent_level) const;
  };

  struct Float : public Value {
    Float(const double& value_) : value(value_) {}
    double value;
    std::string format(unsigned int indent_level) const;
  };

  struct DictDefinition {
    DictDefinition(const Name& key_, const Name& value_)
      : key(key_), value(value_) {}
    Name key;
    Name value;
    std::string format(unsigned int indent_level) const;
  };

  struct Dictionary : public Value {
    std::vector<DictDefinition> definitions;
    std::string format(unsigned int indent_level) const;
  };

  struct Array : public Value {
    std::vector<Name> values;
    std::string format(unsigned int indent_level) const;
  };

  struct Expression {
    virtual ~Expression() {}
    virtual std::string format(unsigned int indent_level) const = 0;
    protected: Expression() {} };

  struct PositionalOutArgument {
    PositionalOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
    std::string format(unsigned int indent_level) const;
  };

  struct OptionalOutArgument {
    OptionalOutArgument(const Name& key_, const Name& variable_)
      : key(key_), variable(variable_) {}
    Name key;
    Name variable;
    std::string format(unsigned int indent_level) const;
  };

  struct ArbitraryOutArgument {
    ArbitraryOutArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format(unsigned int indent_level) const;
  };

  struct KeywordOutArgument {
    KeywordOutArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format(unsigned int indent_level) const;
  };

  struct Call : public Expression {
    PTR<Value> callable;
    std::vector<PositionalOutArgument> left_positional_args;
    boost::optional<ArbitraryOutArgument> left_arbitrary_arg;
    std::vector<PositionalOutArgument> right_positional_args;
    std::vector<OptionalOutArgument> right_optional_args;
    boost::optional<ArbitraryOutArgument> right_arbitrary_arg;
    boost::optional<KeywordOutArgument> right_keyword_arg;
    std::vector<OptionalOutArgument> scoped_optional_args;
    boost::optional<KeywordOutArgument> scoped_keyword_arg;
    PTR<Value> continuation;
    PTR<Value> exception;
    std::string format(unsigned int indent_level) const;
  };

  struct VariableMutation : public Expression {
    VariableMutation(const Name& assignee_, Name value_,
        PTR<Expression> next_expression_)
      : assignee(assignee_), value(value_),
        next_expression(next_expression_) {}
    Name assignee;
    Name value;
    PTR<Expression> next_expression;
    std::string format(unsigned int indent_level) const;
  };

  struct ObjectMutation : public Expression {
    ObjectMutation(const Name& object_, const Name& field_, const Name& value_,
        PTR<Expression> next_expression_)
      : object(object_), field(field_), value(value_),
        next_expression(next_expression_) {}
    Name object;
    Name field;
    Name value;
    PTR<Expression> next_expression;
    std::string format(unsigned int indent_level) const;
  };

  struct PositionalInArgument {
    PositionalInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format(unsigned int indent_level) const;
  };

  struct OptionalInArgument {
    OptionalInArgument(const Name& variable_, const Name& defaultval_)
      : variable(variable_), defaultval(defaultval_) {}
    Name variable;
    Name defaultval;
    std::string format(unsigned int indent_level) const;
  };

  struct ArbitraryInArgument {
    ArbitraryInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format(unsigned int indent_level) const;
  };

  struct KeywordInArgument {
    KeywordInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format(unsigned int indent_level) const;
  };

  struct Callable : public Value {
    PTR<Expression> expression;
  };

  struct Function : public Callable {
    std::vector<PositionalInArgument> left_positional_args;
    boost::optional<ArbitraryInArgument> left_arbitrary_arg;
    std::vector<PositionalInArgument> right_positional_args;
    std::vector<OptionalInArgument> right_optional_args;
    boost::optional<ArbitraryInArgument> right_arbitrary_arg;
    boost::optional<KeywordInArgument> right_keyword_arg;
    std::string format(unsigned int indent_level) const;
  };

  struct Continuation : public Callable {
    std::vector<PositionalInArgument> vars;
    std::string format(unsigned int indent_level) const;
  };

  struct Scope : public Callable {
    std::string format(unsigned int indent_level) const;
  };

  void transform(const std::vector<PTR<cirth::ir::Expression> >& in_ir,
      const ir::Name& in_lastval, PTR<Expression>& out_ir);

}}

#endif
