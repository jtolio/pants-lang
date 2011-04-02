#ifndef __IR_H__
#define __IR_H__

#include "common.h"
#include "ast.h"

namespace cirth {
namespace ir {

  struct Definition; struct VariableMutation; struct ObjectMutation;
  struct ReturnValue;

  struct ExpressionVisitor {
    virtual void visit(Definition*) = 0;
    virtual void visit(VariableMutation*) = 0;
    virtual void visit(ObjectMutation*) = 0;
    virtual void visit(ReturnValue*) = 0;
  };

  struct Field; struct Variable; struct Integer; struct CharString;
  struct ByteString; struct Float; struct Dictionary; struct Array;
  struct Function; struct Scope;

  struct ValueVisitor {
    virtual void visit(Field*) = 0;
    virtual void visit(Variable*) = 0;
    virtual void visit(Integer*) = 0;
    virtual void visit(CharString*) = 0;
    virtual void visit(ByteString*) = 0;
    virtual void visit(Float*) = 0;
    virtual void visit(Dictionary*) = 0;
    virtual void visit(Array*) = 0;
    virtual void visit(Function*) = 0;
    virtual void visit(Scope*) = 0;
  };


  struct Expression {
    virtual ~Expression() {}
    virtual std::string format() const = 0;
    virtual void accept(ExpressionVisitor* visitor) = 0;
    protected: Expression() {} };

  struct Value {
    virtual ~Value() {}
    virtual std::string format() const = 0;
    virtual void accept(ValueVisitor* visitor) = 0;
    protected: Value() {} };

  struct Name {
    Name(const std::string& name_, bool user_provided_, bool scoped_)
      : name(name_), user_provided(user_provided_), scoped(scoped_) {}
    Name(const cirth::ast::Variable& var)
      : name(var.name), user_provided(var.user_provided), scoped(var.scoped) {}
    std::string format() const;
    std::string name;
    bool user_provided;
    bool scoped;
  };

  struct Definition : public Expression {
    Definition(const Name& assignee_, PTR<Value> value_)
      : assignee(assignee_), value(value_) {}
    Name assignee;
    PTR<Value> value;
    std::string format() const;
    void accept(ExpressionVisitor* visitor) { visitor->visit(this); }
  };

  struct VariableMutation : public Expression {
    VariableMutation(const Name& assignee_, PTR<Value> value_)
      : assignee(assignee_), value(value_) {}
    Name assignee;
    PTR<Value> value;
    std::string format() const;
    void accept(ExpressionVisitor* visitor) { visitor->visit(this); }
  };

  struct ObjectMutation : public Expression {
    ObjectMutation(PTR<Value> object_, const Name& field_, PTR<Value> value_)
      : object(object_), field(field_), value(value_) {}
    PTR<Value> object;
    Name field;
    PTR<Value> value;
    std::string format() const;
    void accept(ExpressionVisitor* visitor) { visitor->visit(this); }
  };

  struct Field : public Value {
    Field(const PTR<Value>& object_, const Name& field_)
      : object(object_), field(field_) {}
    std::string format() const;
    PTR<Value> object;
    Name field;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

  struct Variable : public Value {
    Variable(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format() const;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

  struct Integer : public Value {
    Integer(const long long& value_) : value(value_) {}
    long long value;
    std::string format() const;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

  struct CharString : public Value {
    CharString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format() const;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

  struct ByteString : public Value {
    ByteString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format() const;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

  struct Float : public Value {
    Float(const double& value_) : value(value_) {}
    double value;
    std::string format() const;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

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

  struct Call {
    PTR<Value> callable;
    std::vector<PositionalOutArgument> left_positional_args;
    boost::optional<ArbitraryOutArgument> left_arbitrary_arg;
    std::vector<PositionalOutArgument> right_positional_args;
    std::vector<OptionalOutArgument> right_optional_args;
    boost::optional<ArbitraryOutArgument> right_arbitrary_arg;
    boost::optional<KeywordOutArgument> right_keyword_arg;
    std::vector<OptionalOutArgument> scoped_optional_args;
    boost::optional<KeywordOutArgument> scoped_keyword_arg;
    std::string format() const;
  };

  struct ReturnValue : public Expression {
    ReturnValue(const Name& assignee_, PTR<Call> term_)
      : assignee(assignee_), term(term_) {}
    Name assignee;
    PTR<Call> term;
    std::string format() const;
    void accept(ExpressionVisitor* visitor) { visitor->visit(this); }
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
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

  struct Array : public Value {
    std::vector<PTR<Value> > values;
    std::string format() const;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
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
    std::vector<PTR<Expression> > expressions;
    PTR<Value> lastval;
  };

  struct Function : public Callable {
    std::vector<PositionalInArgument> left_positional_args;
    boost::optional<ArbitraryInArgument> left_arbitrary_arg;
    std::vector<PositionalInArgument> right_positional_args;
    std::vector<OptionalInArgument> right_optional_args;
    boost::optional<ArbitraryInArgument> right_arbitrary_arg;
    boost::optional<KeywordInArgument> right_keyword_arg;
    std::string format() const;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

  struct Scope : public Callable {
    std::string format() const;
    void accept(ValueVisitor* visitor) { visitor->visit(this); }
  };

  void convert(const std::vector<PTR<cirth::ast::Expression> >& exps,
      std::vector<PTR<cirth::ir::Expression> >& out,
      PTR<cirth::ir::Value>& lastval);

}}

#endif
