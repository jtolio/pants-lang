#ifndef __IR_H__
#define __IR_H__

#include "common.h"
#include "ast.h"

namespace cirth {
namespace ir {

  struct Assignment {
    virtual ~Assignment() {}
    virtual std::string format() const = 0;
    protected: Assignment() {} };

  struct Value {
    virtual ~Value() {}
    virtual std::string format() const = 0;
    protected: Value() {} };

  struct Assignee {
    virtual ~Assignee() {}
    virtual std::string format() const = 0;
    protected: Assignee() {} };

  struct InArgument {
    virtual ~InArgument() {}
    virtual std::string format() const = 0;
    protected: InArgument() {} };

  struct OutArgument {
    virtual ~OutArgument() {}
    virtual std::string format() const = 0;
    protected: OutArgument() {} };

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

  struct Definition : public Assignment {
    Definition(PTR<Assignee> assignee_, PTR<Value> value_)
      : assignee(assignee_), value(value_) {}
    PTR<Assignee> assignee;
    PTR<Value> value;
    std::string format() const;
  };

  struct Mutation : public Assignment {
    Mutation(PTR<Assignee> assignee_, PTR<Value> value_)
      : assignee(assignee_), value(value_) {}
    PTR<Assignee> assignee;
    PTR<Value> value;
    std::string format() const;
  };

  struct SingleAssignee : public Assignee {
    SingleAssignee(const Name& variable_) : variable(variable_) {}
    std::string format() const;
    Name variable;
  };

  struct IndexAssignee : public Assignee {
    IndexAssignee(const PTR<Value>& array_, const PTR<Value>& index_)
      : array(array_), index(index_) {}
    std::string format() const;
    PTR<Value> array;
    PTR<Value> index;
  };

  struct FieldAssignee : public Assignee {
    FieldAssignee(const PTR<Value>& object_, const Name& field_)
      : object(object_), field(field_) {}
    std::string format() const;
    PTR<Value> object;
    Name field;
  };

  struct Index : public Value {
    Index(const PTR<Value>& array_, const PTR<Value>& index_)
      : array(array_), index(index_) {}
    std::string format() const;
    PTR<Value> array;
    PTR<Value> index;
  };

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

  struct PositionalOutArgument : public OutArgument {
    PositionalOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
    std::string format() const;
  };

  struct OptionalOutArgument : public OutArgument {
    OptionalOutArgument(const Name& key_, const PTR<Value>& variable_)
      : key(key_), variable(variable_) {}
    Name key;
    PTR<Value> variable;
    std::string format() const;
  };

  struct ArbitraryOutArgument : public OutArgument {
    ArbitraryOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
    std::string format() const;
  };

  struct KeywordOutArgument : public OutArgument {
    KeywordOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
    std::string format() const;
  };

  struct Call {
    PTR<Value> function;
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

  struct PositionalInArgument : public InArgument {
    PositionalInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format() const;
  };

  struct OptionalInArgument : public InArgument {
    OptionalInArgument(const Name& variable_, const PTR<Value>& defaultval_)
      : variable(variable_), defaultval(defaultval_) {}
    Name variable;
    PTR<Value> defaultval;
    std::string format() const;
  };

  struct ArbitraryInArgument : public InArgument {
    ArbitraryInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format() const;
  };

  struct KeywordInArgument : public InArgument {
    KeywordInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format() const;
  };

  struct Function : public Value {
    std::vector<PositionalInArgument> left_positional_args;
    boost::optional<ArbitraryInArgument> left_arbitrary_arg;
    std::vector<PositionalInArgument> right_positional_args;
    std::vector<OptionalInArgument> right_optional_args;
    boost::optional<ArbitraryInArgument> right_arbitrary_arg;
    boost::optional<KeywordInArgument> right_keyword_arg;
    std::vector<PTR<Assignment> > assignments;
    Call trailing_call;
    std::string format() const;
  };

  void convert(const std::vector<PTR<cirth::ast::Expression> >& exps,
      std::vector<PTR<Assignment> >& assignments, Call& trailing_call);

}}

#endif
