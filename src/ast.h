#ifndef __AST_H__
#define __AST_H__

#include "common.h"

namespace cirth {
namespace ast {

  struct Expression { virtual ~Expression() {} protected: Expression() {} };
  struct Term { virtual ~Term() {} protected: Term() {} };
  struct Assignee { virtual ~Assignee() {} protected: Assignee() {} };

  struct Variable : public Term {
    Variable() {}
    Variable(const std::string& name_) : name(name_) {}
    std::string name;
  };

  struct VariableAssignee : public Assignee { Variable name; };
  struct LookupAssignee : public Assignee
    { PTR<Term> object; Variable field; };
  struct IndexAssignee : public Assignee
    { PTR<Term> record; PTR<Expression> index; };
  struct AssigneeList : public Assignee
    { std::vector<PTR<Assignee> > assignees; };

  struct Reassignment : public Expression {
    Assignee assignee;
    PTR<Expression> assignment;
  };

  struct Definition : public Expression {
    Assignee assignee;
    PTR<Expression> assignment;
  };

  struct List : public Expression {
    List(const std::vector<PTR<Expression> >& values_) : values(values_) {}
    std::vector<PTR<Expression> > values;
  };

  struct Application : public Expression {
    Application(const std::vector<PTR<Term> >& terms_) : terms(terms_) {}
    std::vector<PTR<Term> > terms;
  };

  struct ListExpansion : public Term {
    ListExpansion(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
  };

  struct SubExpression : public Term {
    SubExpression(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
  };

  struct Call : public Term {
    Call(const PTR<Term>& function_) : function(function_) {}
    PTR<Term> function;
  };

  struct Lookup : public Term {
    Lookup(const PTR<Term>& object_, const Variable& field_)
      : object(object_), field(field_) {}
    PTR<Term> object;
    Variable field;
  };

  struct Index : public Term {
    Index(const PTR<Term>& record_, const std::vector<PTR<Expression> >& index_)
      : record(record_), index(index_) {}
    PTR<Term> record;
    std::vector<PTR<Expression> > index;
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

  struct MapDefinition {
    PTR<Expression> key;
    PTR<Expression> value;
  };

  struct Map : public Term {
    Map(const std::vector<MapDefinition>& values_) : values(values_) {}
    std::vector<MapDefinition> values;
  };
  
  struct VarArg {
    Variable name;
    PTR<Term> subexpression;
  };
  
  struct OptionalArgs {
    boost::optional<VarArg> var_arg;
    std::vector<Variable> optional_args;
  };

  struct HalfArgs {
    // ONE of the following two will be set
    boost::optional<OptionalArgs> optional_args;
    boost::optional<VarArg> var_arg;
    std::vector<Variable> args;
  };

  struct ArgList {
    boost::optional<HalfArgs> leftargs;
    HalfArgs rightargs;
  };

  struct Function : public Term {
    Function(const boost::optional<ArgList>& args_,
        const std::vector<PTR<Expression> >& expressions_)
        : args(args_), expressions(expressions_) {}
    boost::optional<ArgList> args;
    std::vector<PTR<Expression> > expressions;
  };

}}

#endif
