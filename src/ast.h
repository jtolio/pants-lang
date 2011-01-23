#ifndef __AST_H__
#define __AST_H__

#include "common.h"

namespace cirth {
namespace ast {

  struct Expression { virtual ~Expression() {} protected: Expression() {} };
  struct Term { virtual ~Term() {} protected: Term() {} };
  struct Assignee { virtual ~Assignee() {} protected: Assignee() {} };

  struct Variable : public Term {
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

  struct Function : public Term {
    std::vector<PTR<Expression> > expressions;
    Variable left_var_args;
    std::vector<PTR<Expression> > left_var_arg_length;
    std::vector<Variable> left_optional_args;
    std::vector<Variable> left_args;
    std::vector<Variable> right_args;
    std::vector<Variable> right_optional_args;
    Variable right_var_args;
    std::vector<PTR<Expression> > right_var_arg_length;
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

  struct Map : public Term {
    std::vector<std::pair<PTR<Expression>, PTR<Expression> > > values;
  };

}}

#endif
