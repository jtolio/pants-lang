#ifndef __AST_H__
#define __AST_H__

#include "common.h"

namespace jtlang {
namespace ast {

  struct Expression { virtual ~Expression() {} protected: Expression() {} };
  struct Term : public Expression { protected: Term() {} };
  struct Value : public Term { protected: Value() {} };

  struct Program {
    std::vector<PTR<Expression> > expressions;
  };
  
  struct Record : public Expression {
    std::vector<PTR<Expression> > expressions;
  };
  
  struct Application : public Expression {
    std::vector<PTR<Term> > terms;
  };

  struct ListExpansion : public Term {
    std::vector<PTR<Expression> > expressions;
  };
  
  struct SubExpression : public Value {
    std::vector<PTR<Expression> > expressions;
  };
  
  struct Call : public Value {
    PTR<Value> function;
  };
  
  struct Variable : public Value {
    std::string name;
  };

  struct Lookup : public Value {
    PTR<Value> object;
    Variable field;
  };
  
  struct Index : public Value {
    PTR<Value> record;
    std::vector<PTR<Expression> > index;
  };
  
  struct Assignee { protected: Assignee() {} };
  struct VariableAssignee { Variable variable; };
  struct LookupAssignee { Lookup lookup; };
  struct IndexAssignee { Index index; };
  struct AssigneeList { std::vector<PTR<Assignee> > assignees; };
  
  struct Assignment : public Expression { protected: Assignment() {} public:
    PTR<Assignee> assignee;
    PTR<Expression> expression;
  };
 
  struct Reassignment : public Assignment {};
  struct Definition : public Assignment {};

  struct Function : public Value {
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
  
  struct Integer : public Value {
    long long value;
  };
  
  struct CharString : public Value {
    std::string value;
  };
  
  struct ByteString : public Value {
    std::string value;
  };
  
  struct Float : public Value {
    double value;
  };
  
  struct List : public Value {
    std::vector<PTR<Expression> > values;
  };
  
  struct Dictionary : public Value {
    std::vector<std::pair<PTR<Expression>, PTR<Expression> > > values;
  };
  
}}

#endif
