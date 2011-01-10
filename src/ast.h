#ifndef __AST_H__
#define __AST_H__

#include "common.h"

namespace jtlang {
namespace ast {

  union Expression;
  union Term;
  union Assignee;

  struct Program {
    std::vector<Expression> expressions;
  };
  
  enum AsigneeType { VARIABLE, LOOKUP, INDEX, LIST };
  
  struct VariableAssignee {
    AssigneeType type;
    std::string name;
  };
  
  struct LookupAssignee {
    AssigneeType type;
    PTR<Term> object;
    std::string field;
  };
  
  struct IndexAssignee {
    AssigneeType type;
    PTR<Term> record;
    std::vector<PTR<Expression> > index;
  };
  
  struct ListAssignee {
    AssigneeType type;
    std::vector<Assignee> assignees;
  };

  union Assignee {
    AssigneeType type;
    VariableAssignee variable;
    LookupAssignee lookup;
    IndexAssignee index;
    ListAssignee list;
  };

  enum ExpressionType { REASSIGNMENT,
                        DEFINITION,
                        TUPLE,
                        APPLICATION };
  
  struct Assignment {
    ExpressionType type;
    Assignee assignee;
    PTR<Expression> assignment;    
  }
  
  struct Tuple {
    ExpressionType type;
    std::vector<PTR<Expression> > values;
  };
  
  struct Application {
    ExpressionType type;
    std::vector<PTR<Term> > terms;
  }

  union Expression {
    ExpressionType type;
    Assignment assignment;
    Tuple tuple;
    Application application;
  };

  enum TermType { LISTEXPANSION,
                  SUBEXPRESSION,
                  FUNCTION,
                  VARIABLE,
                  CALL,
                  LOOKUP,
                  INDEX,
                  INTEGER,
                  BYTESTRING,
                  CHARSTRING,
                  FLOAT,
                  LIST,
                  MAP };
                  
  struct ListExpansion {
    TermType type;
    std::vector<PTR<Expression> > expressions;
  };
  struct SubExpression {
    TermType type;
    std::vector<PTR<Expression> > expressions;
  };
  struct Variable {
    TermType type;
    std::string name;
  };
  struct Function {
    TermType type;
    std::vector<PTR<Expression> > expressions;
    std::string left_var_args;
    std::vector<PTR<Expression> > left_var_arg_length;
    std::vector<std::string> left_optional_args;
    std::vector<std::string> left_args;
    std::vector<std::string> right_args;
    std::vector<std::string> right_optional_args;
    std::string right_var_args;
    std::vector<PTR<Expression> > right_var_arg_length;
  };
  struct Call {
    TermType type;
    PTR<Term> function;
  };
  struct Lookup {
    TermType type;
    PTR<Term> object;
    std::string field;
  };
  struct Index {
    TermType type;
    PTR<Term> record;
    std::vector<PTR<Expression> > index;
  };
  struct Integer {
    TermType type;
    long long value;
  };
  struct ByteString {
    TermType type;
    std::string value;
  };
  struct CharString {
    TermType type;
    std::string value;
  };
  struct Float {
    TermType type;
    double value;
  };
  struct List {
    TermType type;
    std::vector<PTR<Application> > values;
  };
  struct Map {
    TermType type;
    std::vector<std::pair<PTR<Application>, PTR<Application> > > values;
  };
  
  union Term {
    TermType type;
    ListExpansion listexpansion;
    SubExpression subexpression;
    Function function;
    Variable variable;
    Call call;
    Lookup lookup;
    Index index;
    Integer integer;
    ByteString bytestring;
    CharString charstring;
    Float float_;
    List list;
    Map map;
  };
  
}}

#endif
