#ifndef __PRE_CPS_IR_H__
#define __PRE_CPS_IR_H__

#include "common.h"
#include "ast.h"

namespace cirth {
namespace pre_cps_ir {

  struct Expression {
    virtual ~Expression() {}
    protected: Expression() {} };

  struct Term : public Expression {
    virtual ~Term() {}
    protected: Term() {} };
    
  struct Value : public Term {
    virtual ~Value() {}
    protected: Value() {} };

  struct Assignee {
    virtual ~Assignee() {}
    protected: Assignee() {} };

  struct InArgument {
    virtual ~InArgument() {}
    protected: InArgument() {} };

  struct OutArgument {
    virtual ~OutArgument() {}
    protected: OutArgument() {} };

  struct Name {
    Name(const std::string& name_, bool user_provided_)
      : name(name_), user_provided(user_provided_) {}
    Name(const cirth::ast::Variable& var)
      : name(var.name), user_provided(var.user_provided) {}
    std::string name;
    bool user_provided;
  };

  struct Assignment : public Expression {
    Assignment(PTR<Assignee> assignee_, PTR<Term> term_, bool mutation_)
      : assignee(assignee_), term(term_), mutation(mutation_) {}
    PTR<Assignee> assignee;
    PTR<Term> term;
    bool mutation;
  };

  struct SingleAssignee : public Assignee {
    SingleAssignee(const Name& variable_) : variable(variable_) {}
    Name variable;
  };

  struct IndexAssignee : public Assignee {
    IndexAssignee(const PTR<Value>& array_, const PTR<Value>& index_)
      : array(array_), index(index_) {}
    PTR<Value> array;
    PTR<Value> index;
  };

  struct FieldAssignee : public Assignee {
    FieldAssignee(const PTR<Value>& object_, const Name& field_)
      : object(object_), field(field_) {}
    PTR<Value> object;
    Name field;
  };
  
  struct Index : public Term {
    Index(const PTR<Value>& array_, const PTR<Value>& index_)
      : array(array_), index(index_) {}
    PTR<Value> array;
    PTR<Value> index;
  };

  struct Field : public Term {
    Field(const PTR<Value>& object_, const Name& field_)
      : object(object_), field(field_) {}
    PTR<Value> object;
    Name field;
  };

  struct Variable : public Value {
    Variable(const Name& variable_) : variable(variable_) {}
    Name variable;
  };

  struct Integer : public Value {
    Integer(const long long& value_) : value(value_) {}
    long long value;
  };

  struct CharString : public Value {
    CharString(const std::string& value_) : value(value_) {}
    std::string value;
  };

  struct ByteString : public Value {
    ByteString(const std::string& value_) : value(value_) {}
    std::string value;
  };

  struct Float : public Value {
    Float(const double& value_) : value(value_) {}
    double value;
  };

  struct PositionalOutArgument : public OutArgument {
    PositionalOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
  };

  struct OptionalOutArgument : public OutArgument {
    OptionalOutArgument(const Name& key_, const PTR<Value>& variable_)
      : key(key_), variable(variable_) {}
    Name key;
    PTR<Value> variable;
  };

  struct ArbitraryOutArgument : public OutArgument {
    ArbitraryOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
  };

  struct KeywordOutArgument : public OutArgument {
    KeywordOutArgument(const PTR<Value>& variable_) : variable(variable_) {}
    PTR<Value> variable;
  };

  struct Call : public Term {
    PTR<Value> function;
    std::vector<PositionalOutArgument> left_positional_args;
    std::vector<PositionalOutArgument> right_positional_args;
    std::vector<OptionalOutArgument> right_optional_args;
    boost::optional<ArbitraryOutArgument> right_arbitrary_arg;
    boost::optional<KeywordOutArgument> right_keyword_arg;
    std::vector<OptionalOutArgument> scoped_optional_args;
    boost::optional<KeywordOutArgument> scoped_keyword_arg;
  };
  
  struct DictDefinition {
    DictDefinition(const PTR<Value>& key_, const PTR<Value>& value_)
      : key(key_), value(value_) {}
    PTR<Value> key;
    PTR<Value> value;
  };

  struct Dictionary : public Value {
    std::vector<DictDefinition> definitions;
  };

  struct Array : public Value {
    std::vector<PTR<Value> > values;
  };

  struct PositionalInArgument : public InArgument {
    PositionalInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
  };

  struct OptionalInArgument : public InArgument {
    OptionalInArgument(const Name& variable_, const PTR<Value>& defaultval_)
      : variable(variable_), defaultval(defaultval_) {}
    Name variable;
    PTR<Value> defaultval;
  };

  struct ArbitraryInArgument : public InArgument {
    ArbitraryInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
  };

  struct KeywordInArgument : public InArgument {
    KeywordInArgument(const Name& variable_) : variable(variable_) {}
    Name variable;
  };

  struct Function : public Value {
    Function(bool full_function_) : full_function(full_function_) {}
    std::vector<PositionalInArgument> left_positional_args;
    std::vector<PositionalInArgument> right_positional_args;
    std::vector<OptionalInArgument> right_optional_args;
    boost::optional<ArbitraryInArgument> right_arbitrary_arg;
    boost::optional<KeywordInArgument> right_keyword_arg;
    std::vector<PTR<Expression> > expressions;
    bool full_function;
  };

  void convert(const std::vector<PTR<cirth::ast::Expression> >& exps,
      std::vector<PTR<cirth::pre_cps_ir::Expression> >& out);

}}

#endif
