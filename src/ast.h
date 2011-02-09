#ifndef __AST_H__
#define __AST_H__

#include "common.h"
#include <boost/optional.hpp>

namespace cirth {
namespace ast {

  struct Expression {
    virtual ~Expression() {} 
    virtual std::string format() const = 0;
    protected: Expression() {} };
  
  struct Term {
    virtual ~Term() {}
    virtual std::string format() const = 0;
    protected: Term() {} };
  
  struct Value {
    virtual ~Value() {}
    virtual std::string format() const = 0;
    protected: Value() {} };
  
  struct Trailer {
    virtual ~Trailer() {}
    virtual std::string format() const = 0;
    protected: Trailer() {} };

  struct Assignee {
    virtual ~Assignee() {}
    virtual std::string format() const = 0;
    protected: Assignee() {} };

  struct List : public Expression {
    List(const std::vector<PTR<Expression> >& values_) : values(values_) {}
    std::vector<PTR<Expression> > values;
    std::string format() const;
  };

  struct Application : public Expression {
    Application(const std::vector<PTR<Term> >& terms_) : terms(terms_) {}
    std::vector<PTR<Term> > terms;
    std::string format() const;
  };
  
  struct Assignment : public Expression {
    Assignment(PTR<Assignee> assignee_, PTR<Expression> exp_)
      : assignee(assignee_), exp(exp_) {}
    PTR<Assignee> assignee;
    PTR<Expression> exp;
    virtual std::string name() const = 0;
    std::string format() const;
  };
  
  struct Mutation : public Assignment {
    std::string name() const { return "Mutation"; }
  };
  
  struct Definition : public Assignment {
    std::string name() const { return "Definition"; }
  };

  struct ListExpansion : public Term {
    ListExpansion(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
  };
  
  struct FullValue : public Term {
    FullValue(PTR<Value> value_, const std::vector<PTR<Trailer> >& trailers_)
      : value(value_), trailers(trailers_) {}
    PTR<Value> value;
    std::vector<PTR<Trailer> > trailers;
    std::string format() const;
  };
  
  struct Variable : public Value {
    Variable() : user_provided(true) {}
    Variable(const std::string& name_) : name(name_), user_provided(true) {}
    Variable(const std::string& name_, bool user_provided_)
      : name(name_), user_provided(user_provided_) {}
    std::string name;
    bool user_provided;
    std::string format() const;
  };  

  struct SubExpression : public Value {
    SubExpression(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
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

  struct MapDefinition {
    PTR<Expression> key;
    PTR<Expression> value;
    std::string format() const;
  };

  struct Map : public Value {
    Map(const std::vector<MapDefinition>& values_) : values(values_) {}
    std::vector<MapDefinition> values;
    std::string format() const;
  };
  
  struct VarArg {
    Variable name;
    PTR<Value> subexpression;
  };
  
  struct OptionalArgs {
    boost::optional<VarArg> var_arg;
    std::vector<Variable> optional_args;
  };

  struct HalfArgs {
    boost::optional<OptionalArgs> optional_args;
    boost::optional<VarArg> var_arg;
    std::vector<Variable> args;
  };

  struct ArgList {
    boost::optional<HalfArgs> leftargs;
    HalfArgs rightargs;
  };

  struct Function : public Value {
    Function(const boost::optional<ArgList>& args,
        const std::vector<PTR<Expression> >& expressions_);
    std::vector<Variable> right_args;
    std::vector<Variable> right_optional_args;
    boost::optional<VarArg> right_var_arg;
    std::vector<Variable> left_args;
    std::vector<Variable> left_optional_args;
    boost::optional<VarArg> left_var_arg;
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
  };

  struct Call : public Trailer {
    std::string format() const;
  };

  struct Field : public Trailer {
    Field(const Variable& variable_) : variable(variable_) {}
    Variable variable;
    std::string format() const;
  };
  
  struct Index : public Trailer {
    Index(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
  };
  
  struct VariableAssignee : public Assignee {
    Variable name;
    std::string format() const;
  };
  
  struct FieldAssignee : public Assignee {
    PTR<Term> fullvalue;
    Variable field;
    std::string format() const;
  };
  
  struct IndexAssignee : public Assignee {
    PTR<Term> fullvalue;
    std::vector<PTR<Expressio> > expressions;
    std::string format() const;
  };
  
  struct AssigneeList : public Assignee {
    AssigneeList(const std::vector<PTR<Assignee> > assignees_)
      : assignees(assignees_) {}
    std::vector<PTR<Assignee> > assignees;
    std::string format() const;
  };
  
}}

#endif
