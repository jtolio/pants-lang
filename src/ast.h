#ifndef __AST_H__
#define __AST_H__

#include "common.h"

namespace cirth {
namespace ast {

  struct Expression { virtual ~Expression() {} protected: Expression() {} };
  struct Term { virtual ~Term() {} protected: Term() {} };
  struct Value { virtual ~Value() {} protected: Value() {} };
  struct Trailer { virtual ~Trailer() {} protected: Trailer() {} };

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
  
  struct FullValue : public Term {
    FullValue(PTR<Value> value_, const std::vector<PTR<Trailer> >& trailers_)
      : value(value_), trailers(trailers_) {}
    PTR<Value> value;
    std::vector<PTR<Trailer> > trailers;
  };
  
  struct Variable : public Value {
    Variable() {}
    Variable(const std::string& name_) : name(name_) {}
    std::string name;
  };  

  struct SubExpression : public Value {
    SubExpression(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
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

  struct MapDefinition {
    PTR<Expression> key;
    PTR<Expression> value;
  };

  struct Map : public Value {
    Map(const std::vector<MapDefinition>& values_) : values(values_) {}
    std::vector<MapDefinition> values;
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
    // ONE of the following two will be set
    boost::optional<OptionalArgs> optional_args;
    boost::optional<VarArg> var_arg;
    std::vector<Variable> args;
  };

  struct ArgList {
    boost::optional<HalfArgs> leftargs;
    HalfArgs rightargs;
  };

  struct Function : public Value {
    Function(const boost::optional<ArgList>& args_,
        const std::vector<PTR<Expression> >& expressions_)
        : args(args_), expressions(expressions_) {}
    boost::optional<ArgList> args;
    std::vector<PTR<Expression> > expressions;
  };

  struct Call : public Trailer {};

  struct Lookup : public Trailer {
    Lookup(const Variable& variable_) : variable(variable_) {}
    Variable variable;
  };
  
  struct Index : public Trailer {
    Index(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
  };
  
}}

#endif
