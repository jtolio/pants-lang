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

  struct Assignee {
    virtual ~Assignee() {}
    protected: Assignee() {} };

  struct Argument {
    virtual ~Argument() {}
    protected: Argument() {} };

  struct Variable {
    Variable(const std::string& name_, bool user_provided_)
      : name(name_), user_provided(user_provided_) {}
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
    SingleAssignee(const Variable& variable_) : variable(variable_) {}
    Variable variable;
  };

  struct IndexAssignee : public Assignee {
    IndexAssignee(const Variable& array_, const Variable& index_)
      : array(array_), index(index_) {}
    Variable array;
    Variable index;
  };

  struct FieldAssignee : public Assignee {
    FieldAssignee(const Variable& object_, const Variable& field_)
      : object(object_), field(field_) {}
    Variable object;
    Variable field;
  };

  struct Index : public Term {
    Index(const Variable& array_, const Variable& index_)
      : array(array_), index(index_) {}
    Variable array;
    Variable index;
  };

  struct Field : public Term {
    Field(const Variable& object_, const Variable& field_)
      : object(object_), field(field_) {}
    Variable object;
    Variable field;
  };

  struct VariableTerm : public Term {
    VariableTerm(const Variable& variable_) : variable(variable_) {}
    Variable variable;
  };

  struct Integer : public Term {
    Integer(const long long& value_) : value(value_) {}
    long long value;
  };

  struct CharString : public Term {
    CharString(const std::string& value_) : value(value_) {}
    std::string value;
  };

  struct ByteString : public Term {
    ByteString(const std::string& value_) : value(value_) {}
    std::string value;
  };

  struct Float : public Term {
    Float(const double& value_) : value(value_) {}
    double value;
  };

  struct Call : public Term {
    Call(const Variable& function_,
        const std::vector<PTR<Argument> >& left_args_,
        const std::vector<PTR<Argument> >& right_args_,
        const std::vector<PTR<Argument> >& scoped_args_)
      : function(function_), left_args(left_args_), right_args(right_args_),
        scoped_args(scoped_args_) {}
    Variable function;
    std::vector<PTR<Argument> > left_args;
    std::vector<PTR<Argument> > right_args;
    std::vector<PTR<Argument> > scoped_args;
  };

  struct DictDefinition {
    DictDefinition(const Variable& key_, const Variable& value_)
      : key(key_), value(value_) {}
    Variable key;
    Variable value;
  };

  struct Dictionary : public Term {
    Dictionary(const std::vector<DictDefinition>& definitions_)
      : definitions(definitions_) {}
    std::vector<DictDefinition> definitions;
  };

  struct Array : public Term {
    Array(const std::vector<Variable>& values_) : values(values_) {}
    std::vector<Variable> values;
  };

  struct Function : public Term {
    Function(const std::vector<PTR<Argument> >& left_args_,
             const std::vector<PTR<Argument> >& right_args_,
             const std::vector<PTR<Expression> >& expressions_)
      : left_args(left_args_), right_args(right_args_),
        expressions(expressions_) {}
    std::vector<PTR<Argument> > left_args;
    std::vector<PTR<Argument> > right_args;
    std::vector<PTR<Expression> > expressions;
  };

  struct PositionalArgument : public Argument {
    PositionalArgument(const Variable& variable_) : variable(variable_) {}
    Variable variable;
  };

  struct OptionalArgument : public Argument {
    OptionalArgument(const Variable& key_, const Variable& variable_)
      : key(key_), variable(variable_) {}
    Variable key;
    Variable variable;
  };

  struct ArbitraryArgument : public Argument {
    ArbitraryArgument(const Variable& variable_) : variable(variable_) {}
    Variable variable;
  };

  struct KeywordArgument : public Argument {
    KeywordArgument(const Variable& variable_) : variable(variable_) {}
    Variable variable;
  };

  bool convert(const std::vector<PTR<cirth::ast::Expression> >& exps,
      std::vector<PTR<cirth::pre_cps_ir::Expression> >& out);

}}

#endif
