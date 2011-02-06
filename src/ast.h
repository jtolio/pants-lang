#ifndef __AST_H__
#define __AST_H__

#include "common.h"

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

  struct List : public Expression {
    List(const std::vector<PTR<Expression> >& values_) : values(values_) {}
    std::vector<PTR<Expression> > values;

    std::string format() const {
      std::ostringstream os;
      os << "List(";
      for(unsigned int i = 0; i < values.size(); ++i) {
        if(i > 0) os << ", ";
        os << values[i]->format();
      }
      os << ")";
      return os.str();
    }
  };

  struct Application : public Expression {
    Application(const std::vector<PTR<Term> >& terms_) : terms(terms_) {}
    std::vector<PTR<Term> > terms;

    std::string format() const {
      std::ostringstream os;
      os << "Application(";
      for(unsigned int i = 0; i < terms.size(); ++i) {
        if(i > 0) os << ", ";
        os << terms[i]->format();
      }
      os << ")";
      return os.str();
    }
  };

  struct ListExpansion : public Term {
    ListExpansion(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
    std::string format() const {
      std::ostringstream os;
      os << "ListExpansion(";
      for(unsigned int i = 0; i < expressions.size(); ++i) {
        if(i > 0) os << ", ";
        os << expressions[i]->format();
      }
      os << ")";
      return os.str();
    }
  };
  
  struct FullValue : public Term {
    FullValue(PTR<Value> value_, const std::vector<PTR<Trailer> >& trailers_)
      : value(value_), trailers(trailers_) {}
    PTR<Value> value;
    std::vector<PTR<Trailer> > trailers;
    std::string format() const {
      std::ostringstream os;
      os << "FullValue(" << value->format();
      for(unsigned int i = 0; i < trailers.size(); ++i) {
        os << ", " << trailers[i]->format();
      }
      os << ")";
      return os.str();
    }
  };
  
  struct Variable : public Value {
    Variable() {}
    Variable(const std::string& name_) : name(name_) {}
    std::string name;
    std::string format() const {
      std::ostringstream os;
      os << "Variable(" << name << ")";
      return os.str();
    }
  };  

  struct SubExpression : public Value {
    SubExpression(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
    std::string format() const {
      std::ostringstream os;
      os << "SubExpression(";
      for(unsigned int i = 0; i < expressions.size(); ++i) {
        if(i > 0) os << ", ";
        os << expressions[i]->format();
      }
      os << ")";
      return os.str();
    }
  };

  struct Integer : public Value {
    Integer(const long long& value_) : value(value_) {}
    long long value;
    std::string format() const {
      std::ostringstream os;
      os << "Integer(" << value << ")";
      return os.str();
    }
  };

  struct CharString : public Value {
    CharString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format() const {
      std::ostringstream os;
      os << "CharString(" << value << ")";
      return os.str();
    }
  };

  struct ByteString : public Value {
    ByteString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format() const {
      std::ostringstream os;
      os << "ByteString(" << value << ")";
      return os.str();
    }
  };

  struct Float : public Value {
    Float(const double& value_) : value(value_) {}
    double value;
    std::string format() const {
      std::ostringstream os;
      os << "Float(" << value << ")";
      return os.str();
    }
  };

  struct MapDefinition {
    PTR<Expression> key;
    PTR<Expression> value;
    std::string format() const {
      std::ostringstream os;
      os << "MapDefinition(" << key->format() << ", " << value->format() << ")";
      return os.str();
    }
  };

  struct Map : public Value {
    Map(const std::vector<MapDefinition>& values_) : values(values_) {}
    std::vector<MapDefinition> values;
    std::string format() const {
      std::ostringstream os;
      os << "Map(";
      for(unsigned int i = 0; i < values.size(); ++i) {
        if(i > 0) os << ", ";
        os << values[i].format();
      }
      os << ")";
      return os.str();
    }
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
    Function(const boost::optional<ArgList>& args,
        const std::vector<PTR<Expression> >& expressions_)
        : expressions(expressions_) {
      if(!args) return;
      right_args = args->rightargs.args;
      if(!!args->rightargs.optional_args) {
        assert(!args->rightargs.var_arg);
        right_optional_args = args->rightargs.optional_args->optional_args;
        right_var_arg = args->rightargs.optional_args->var_arg;
      } else if(!!args->rightargs.var_arg) {
        right_var_arg = args->rightargs.var_arg;
      }
      if(!args->leftargs) return;
      left_args = args->leftargs->args;
      if(!!args->leftargs->optional_args) {
        assert(!args->leftargs->var_arg);
        left_optional_args = args->leftargs->optional_args->optional_args;
        left_var_arg = args->leftargs->optional_args->var_arg;
      } else if(!!args->leftargs->var_arg) {
        left_var_arg = args->leftargs->var_arg;
      }
    }
    std::vector<Variable> right_args;
    std::vector<Variable> right_optional_args;
    boost::optional<VarArg> right_var_arg;
    std::vector<Variable> left_args;
    std::vector<Variable> left_optional_args;
    boost::optional<VarArg> left_var_arg;
    std::vector<PTR<Expression> > expressions;
    std::string format() const {
      std::ostringstream os;
      os << "Function(";
      os << "LeftArgs(";
      for(unsigned int i = 0; i < left_args.size(); ++i) {
        if(i > 0) os << ", ";
        os << left_args[i].format();
      }
      os << "), LeftOptionalArgs(";
      for(unsigned int i = 0; i < left_optional_args.size(); ++i) {
        if(i > 0) os << ", ";
        os << left_optional_args[i].format();
      }
      os << "), ";
      if(!!left_var_arg)
        os << "LeftVariadicArg(" << left_var_arg->name.format() << ", " <<
            left_var_arg->subexpression->format() << "), ";
      os << "RightArgs(";
      for(unsigned int i = 0; i < right_args.size(); ++i) {
        if(i > 0) os << ", ";
        os << right_args[i].format();
      }
      os << "), RightOptionalArgs(";
      for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
        if(i > 0) os << ", ";
        os << right_optional_args[i].format();
      }
      os << "), ";
      if(!!right_var_arg)
        os << "RightVariadicArg(" << right_var_arg->name.format() << ", " <<
            right_var_arg->subexpression->format() << "), ";
      os << "Expressions(";
      for(unsigned int i = 0; i < expressions.size(); ++i) {
        if(i > 0) os << ", ";
        os << expressions[i]->format();
      }
      os << "))";
      return os.str();
    }
  };

  struct Call : public Trailer {
    std::string format() const { return "Call()"; }
  };

  struct Lookup : public Trailer {
    Lookup(const Variable& variable_) : variable(variable_) {}
    Variable variable;
    std::string format() const {
      std::ostringstream os;
      os << "Lookup(" << variable.format() << ")";
      return os.str();
    }
  };
  
  struct Index : public Trailer {
    Index(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
    std::string format() const {
      std::ostringstream os;
      os << "Index(";
      for(unsigned int i = 0; i < expressions.size(); ++i) {
        if(i > 0) os << ", ";
        os << expressions[i]->format();
      }
      os << ")";
      return os.str();
    }
  };
  
}}

#endif
