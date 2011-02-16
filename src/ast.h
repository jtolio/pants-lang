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
  
  struct Value {
    virtual ~Value() {}
    virtual std::string format() const = 0;
    protected: Value() {} };
  
  struct ValueModifier {
    virtual ~ValueModifier() {}
    virtual std::string format() const = 0;
    protected: ValueModifier() {} };

  struct InArgument {
    virtual ~InArgument() {}
    virtual std::string format() const = 0;
    protected: InArgument() {} };

  struct OutArgument {
    virtual ~OutArgument() {}
    virtual std::string format() const = 0;
    protected: OutArgument() {} };
    
  struct Term {
    Term(const std::vector<PTR<ValueModifier> >& headers_,
        PTR<Value> value_, const std::vector<PTR<ValueModifier> >& trailers_)
      : value(value_), headers(headers_), trailers(trailers_) {}
    PTR<Value> value;
    std::vector<PTR<ValueModifier> > headers;
    std::vector<PTR<ValueModifier> > trailers;
    std::string format() const;
  };

  struct Application : public Expression {
    Application(const std::vector<PTR<Term> >& terms_) : terms(terms_) {}
    std::vector<PTR<Term> > terms;
    std::string format() const;
  };

  struct Assignee {
    Assignee(PTR<Term> term_) : term(term_) {}
    PTR<Term> term;
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

  struct DictDefinition {
    PTR<Expression> key;
    PTR<Expression> value;
    std::string format() const;
  };

  struct Dictionary : public Value {
    Dictionary(const std::vector<DictDefinition>& values_);
    std::vector<DictDefinition> values;
    std::string format() const;
  };
  
  struct Array : public Value {
    Array(const std::vector<PTR<Expression> >& values_) : values(values_) {}
    std::vector<PTR<Expression> > values;
    std::string format() const;
  };
  
  struct RequiredInArgument : public InArgument {
    RequiredInArgument(const Variable& name_) : name(name_) {}
    Variable name;
    std::string format() const;
  };
  
  struct OptionalInArgument : public InArgument {
    OptionalInArgument(const Variable& name_, PTR<Term> value_)
      : name(name_), value(value_) {}
    Variable name;
    PTR<Term> value;
    std::string format() const;
  };
  
  struct ArbitraryInArgument : public InArgument {
    ArbitraryInArgument(const Variable& name_) : name(name_) {}
    Variable name;
    std::string format() const;
  };
  
  struct KeywordInArgument : public InArgument {
    KeywordInArgument(const Variable& name_) : name(name_) {}
    Variable name;
    std::string format() const;
  };
  
  struct InArgList {
    boost::optional<std::vector<PTR<InArgument> > > left_args;
    std::vector<PTR<InArgument> > right_args;
  };
  
  struct HalfArgs {
    std::vector<RequiredInArgument> required_args;
    std::vector<OptionalInArgument> optional_args;
    boost::optional<ArbitraryInArgument> arbitrary_arg;
    boost::optional<KeywordInArgument> keyword_arg;  
    std::string format() const;
  };
  
  struct Function : public Value {
    Function(const boost::optional<InArgList>& args_,
        const std::vector<PTR<Expression> >& expressions_);
    HalfArgs left;
    HalfArgs right;
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
  };

  struct OpenCall : public ValueModifier {
    std::string format() const;
  };

  struct RequiredOutArgument : public OutArgument {
    RequiredOutArgument(PTR<Expression> application_)
      : application(application_) {}
    PTR<Expression> application;
    std::string format() const;
  };
  
  struct OptionalOutArgument : public OutArgument {
    OptionalOutArgument(const Variable& name_, PTR<Expression> application_)
      : name(name_), application(application_) {}
    Variable name;
    PTR<Expression> application;
    std::string format() const;
  };
  
  struct ArbitraryOutArgument : public OutArgument {
    ArbitraryOutArgument(const std::vector<PTR<Expression> >& array_)
      : array(array_) {}
    std::vector<PTR<Expression> > array;
    std::string format() const;
  };
  
  struct KeywordOutArgument : public OutArgument {
    KeywordOutArgument(const std::vector<PTR<Expression> >& object_)
      : object(object_) {}
    std::vector<PTR<Expression> > object;
    std::string format() const;
  };
  
  struct ClosedCall : public ValueModifier {
    ClosedCall(
        const boost::optional<std::vector<PTR<OutArgument> > >& left_args_,
        const std::vector<PTR<OutArgument> >& right_args_,
        const boost::optional<std::vector<PTR<OutArgument> > >& scoped_args_);
    std::vector<PTR<OutArgument> > left_args;
    std::vector<PTR<OutArgument> > right_args;
    std::vector<PTR<OutArgument> > scoped_args;
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
  
}}

#endif
