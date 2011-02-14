#ifndef __AST_H__
#define __AST_H__

#include "common.h"
#include <boost/optional.hpp>

namespace cirth {
namespace ast {

  struct expectation_failure : std::runtime_error {
    expectation_failure(const std::string& msg_) throw ()
      : std::runtime_error(msg_) {}
    ~expectation_failure() throw() {}
  };

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
  
  struct ValueModifier {
    virtual ~ValueModifier() {}
    virtual std::string format() const = 0;
    protected: ValueModifier() {} };

  struct Assignee {
    virtual ~Assignee() {}
    virtual std::string format() const = 0;
    protected: Assignee() {} };

  struct Argument {
    virtual ~Argument() {}
    virtual std::string format() const = 0;
    protected: Argument() {} };

  struct List : public Expression {
    List(const std::vector<PTR<Expression> >& values_);
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
    Mutation(PTR<Assignee> a, PTR<Expression> e) : Assignment(a, e) {}
    std::string name() const { return "Mutation"; }
  };
  
  struct Definition : public Assignment {
    Definition(PTR<Assignee> a, PTR<Expression> e) : Assignment(a, e) {}
    std::string name() const { return "Definition"; }
  };

  struct ListExpansion : public Term {
    ListExpansion(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
  };
  
  struct FullValue : public Term {
    FullValue(const std::vector<PTR<ValueModifier> >& headers_,
        PTR<Value> value_, const std::vector<PTR<ValueModifier> >& trailers_)
      : value(value_), headers(headers_), trailers(trailers_) {}
    PTR<Value> value;
    std::vector<PTR<ValueModifier> > headers;
    std::vector<PTR<ValueModifier> > trailers;
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
    Map(const std::vector<MapDefinition>& values_);
    std::vector<MapDefinition> values;
    std::string format() const;
  };
  
  struct RequiredArgument : public Argument {
    RequiredArgument(const Variable& name_) : name(name_) {}
    Variable name;
    std::string format() const;
  };
  
  struct OptionalArgument : public Argument {
    OptionalArgument(const Variable& name_, PTR<Term> value_)
      : name(name_), value(value_) {}
    Variable name;
    PTR<Term> value;
    std::string format() const;
  };
  
  struct ArbitraryArgument : public Argument {
    ArbitraryArgument(const Variable& name_) : name(name_) {}
    Variable name;
    std::string format() const;
  };
  
  struct KeywordArgument : public Argument {
    KeywordArgument(const Variable& name_) : name(name_) {}
    Variable name;
    std::string format() const;
  };
  
  struct ArgList {
    boost::optional<std::vector<PTR<Argument> > > left_args;
    std::vector<PTR<Argument> > right_args;
  };
  
  struct Function : public Value {
    Function(const boost::optional<ArgList>& args_,
        const std::vector<PTR<Expression> >& expressions_);
    std::vector<RequiredArgument> left_required_args;
    std::vector<OptionalArgument> left_optional_args;
    boost::optional<ArbitraryArgument> left_arbitrary_arg;
    boost::optional<KeywordArgument> left_keyword_arg;
    std::vector<RequiredArgument> right_required_args;
    std::vector<OptionalArgument> right_optional_args;
    boost::optional<ArbitraryArgument> right_arbitrary_arg;
    boost::optional<KeywordArgument> right_keyword_arg;
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
  };

  struct OpenCall : public ValueModifier {
    std::string format() const;
  };

  struct ClosedCall : public ValueModifier {
    ClosedCall(const std::vector<PTR<Expression> >& arguments_);
    std::vector<PTR<Expression> > arguments;
    std::string format() const;
  };

  struct Field : public ValueModifier {
    Field(const Variable& variable_) : variable(variable_) {}
    Variable variable;
    std::string format() const;
  };
  
  struct Index : public ValueModifier {
    Index(const std::vector<PTR<Expression> >& expressions_)
      : expressions(expressions_) {}
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
  };
  
  struct SingleAssignee : public Assignee {
    SingleAssignee(PTR<Term> fullvalue_) : fullvalue(fullvalue_) {}
    PTR<Term> fullvalue;
    std::string format() const;
  };
  
  struct AssigneeList : public Assignee {
    AssigneeList(const std::vector<PTR<Assignee> >& assignees_);
    std::vector<PTR<Assignee> > assignees;
    std::string format() const;
  };
  
}}

#endif
