#ifndef __CPS_TRANSFORM_H__
#define __CPS_TRANSFORM_H__

#include "common.h"
#include "ir.h"

namespace cirth {
namespace cps {

  typedef cirth::ir::Name Name;

  struct Value {
    virtual ~Value() {}
    virtual std::string format() const = 0;
    protected: Value() {} };

  struct Field : public Value {
    Field(const PTR<Value>& object_, const Name& field_)
      : object(object_), field(field_) {}
    std::string format() const;
    PTR<Value> object;
    Name field;
  };

  struct Variable : public Value {
    Variable(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format() const;
  };

  struct Integer : public Value {
    Integer(const long long& value_) : value(value_) {}
    long long value;
    std::string format() const;
  };

  struct CharString : public Value {
    CharString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format() const;
  };

  struct ByteString : public Value {
    ByteString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format() const;
  };

  struct Float : public Value {
    Float(const double& value_) : value(value_) {}
    double value;
    std::string format() const;
  };

  struct DictDefinition {
    DictDefinition(const PTR<Value>& key_, const PTR<Value>& value_)
      : key(key_), value(value_) {}
    PTR<Value> key;
    PTR<Value> value;
    std::string format() const;
  };

  struct Dictionary : public Value {
    std::vector<DictDefinition> definitions;
    std::string format() const;
  };

  struct Array : public Value {
    std::vector<PTR<Value> > values;
    std::string format() const;
  };

  struct Expression {
    virtual ~Expression() {}
    virtual std::string format() const = 0;
    protected: Expression() {} };

  struct PositionalOutArgument {
    PositionalOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
    std::string format() const;
  };

  struct OptionalOutArgument {
    OptionalOutArgument(const Name& key_, const PTR<Value>& variable_)
      : key(key_), variable(variable_) {}
    Name key;
    PTR<Value> variable;
    std::string format() const;
  };

  struct ArbitraryOutArgument {
    ArbitraryOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
    std::string format() const;
  };

  struct KeywordOutArgument {
    KeywordOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
    std::string format() const;
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
    std::string format() const;
  };

  struct VariableMutation : public Expression {
    VariableMutation(const Name& assignee_, PTR<Value> value_,
        PTR<Expression> next_expression_)
      : assignee(assignee_), value(value_),
        next_expression(next_expression_) {}
    Name assignee;
    PTR<Value> value;
    PTR<Expression> next_expression;
    std::string format() const;
  };

  struct ObjectMutation : public Expression {
    ObjectMutation(PTR<Value> object_, const Name& field_, PTR<Value> value_,
        PTR<Expression> next_expression_)
      : object(object_), field(field_), value(value_),
        next_expression(next_expression_) {}
    PTR<Value> object;
    Name field;
    PTR<Value> value;
    PTR<Expression> next_expression;
    std::string format() const;
  };

  struct PositionalInArgument {
    PositionalInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format() const;
  };

  struct OptionalInArgument {
    OptionalInArgument(const Name& variable_, const PTR<Value>& defaultval_)
      : variable(variable_), defaultval(defaultval_) {}
    Name variable;
    PTR<Value> defaultval;
    std::string format() const;
  };

  struct ArbitraryInArgument {
    ArbitraryInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format() const;
  };

  struct KeywordInArgument {
    KeywordInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format() const;
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
    std::string format() const;
  };

  struct Continuation : public Callable {
    Continuation(const Name& rv_) : rv(rv_) {}
    Name rv;
    std::string format() const;
  };

  struct Scope : public Callable {
    std::string format() const;
  };

  void transform(const std::vector<PTR<cirth::ir::Expression> >& in_ir,
      const PTR<cirth::ir::Value>& in_lastval, PTR<Expression>& out_ir);

}}

#endif
