#ifndef __AST_H__
#define __AST_H__

#include "common.h"
#include <boost/optional.hpp>

namespace cirth {
namespace ast {

  struct Term; struct Application; struct Assignee; struct Mutation;
  struct Definition; struct Variable; struct SubExpression; struct Integer;
  struct CharString; struct ByteString; struct Float; struct Dictionary;
  struct DictDefinition; struct Array; struct Function; struct Index;
  struct RequiredInArgument; struct OptionalInArgument;
  struct ArbitraryInArgument; struct KeywordInArgument;
  struct RequiredOutArgument; struct OptionalOutArgument;
  struct ArbitraryOutArgument; struct KeywordOutArgument;
  struct HalfArgs; struct OpenCall; struct ClosedCall; struct Field;

  struct AstVisitor {
    virtual void visit(Term*) = 0;
    virtual void visit(Application*) = 0;
    virtual void visit(Mutation*) = 0;
    virtual void visit(Definition*) = 0;
    virtual void visit(Variable*) = 0;
    virtual void visit(SubExpression*) = 0;
    virtual void visit(Integer*) = 0;
    virtual void visit(CharString*) = 0;
    virtual void visit(ByteString*) = 0;
    virtual void visit(Float*) = 0;
    virtual void visit(Dictionary*) = 0;
    virtual void visit(Array*) = 0;
    virtual void visit(Function*) = 0;
    virtual void visit(OpenCall*) = 0;
    virtual void visit(ClosedCall*) = 0;
    virtual void visit(Field*) = 0;
    virtual void visit(Index*) = 0;
  };

  struct Expression {
    virtual ~Expression() {}
    virtual std::string format() const = 0;
    virtual void accept(AstVisitor* visitor) = 0;
    protected: Expression() {} };

  struct Value {
    virtual ~Value() {}
    virtual std::string format() const = 0;
    virtual void accept(AstVisitor* visitor) = 0;
    protected: Value() {} };

  struct ValueModifier {
    virtual ~ValueModifier() {}
    virtual std::string format() const = 0;
    virtual void accept(AstVisitor* visitor) = 0;
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
        PTR<Value> value_, const std::vector<PTR<ValueModifier> >& trailers_);
    PTR<Value> value;
    std::vector<PTR<ValueModifier> > trailers;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
    std::string format() const;
  };

  struct Application : public Expression {
    Application(const std::vector<PTR<Term> >& terms_) : terms(terms_) {}
    std::vector<PTR<Term> > terms;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
    std::string format() const;
  };

  struct Assignee {
    Assignee(PTR<Term> term_) : term(term_) {}
    PTR<Term> term;
    std::string format() const;
  };

  struct Assignment : public Expression {
    PTR<Assignee> assignee;
    PTR<Expression> exp;
    virtual std::string name() const = 0;
    std::string format() const;
    protected:
    Assignment(PTR<Assignee> assignee_, PTR<Expression> exp_)
      : assignee(assignee_), exp(exp_) {}
  };

  struct Mutation : public Assignment {
    Mutation(PTR<Assignee> a, PTR<Expression> e) : Assignment(a, e) {}
    std::string name() const { return "Mutation"; }
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct Definition : public Assignment {
    Definition(PTR<Assignee> a, PTR<Expression> e) : Assignment(a, e) {}
    std::string name() const { return "Definition"; }
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct Variable : public Value {
    Variable() : user_provided(true) {}
    Variable(const std::string& name_) : name(name_), user_provided(true) {}
    Variable(const std::string& name_, bool user_provided_)
      : name(name_), user_provided(user_provided_) {}
    std::string name;
    bool user_provided;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct SubExpression : public Value {
    SubExpression(const std::vector<PTR<Expression> >& expressions_);
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct Integer : public Value {
    Integer(const long long& value_) : value(value_) {}
    long long value;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct CharString : public Value {
    CharString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct ByteString : public Value {
    ByteString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct Float : public Value {
    Float(const double& value_) : value(value_) {}
    double value;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
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
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct Array : public Value {
    Array(const std::vector<PTR<Expression> >& values_);
    std::vector<PTR<Expression> > values;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct RequiredInArgument : public InArgument {
    RequiredInArgument(const Variable& name_) : name(name_) {}
    Variable name;
    std::string format() const;
  };

  struct OptionalInArgument : public InArgument {
    OptionalInArgument(const Variable& name_, PTR<Expression> application_)
      : name(name_), application(application_) {}
    Variable name;
    PTR<Expression> application;
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
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct OpenCall : public ValueModifier {
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
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
    ArbitraryOutArgument(const std::vector<PTR<Expression> >& array_);
    std::vector<PTR<Expression> > array;
    std::string format() const;
  };

  struct KeywordOutArgument : public OutArgument {
    KeywordOutArgument(const std::vector<PTR<Expression> >& object_);
    std::vector<PTR<Expression> > object;
    std::string format() const;
  };

  struct ClosedCall : public ValueModifier {
    ClosedCall() {}
    ClosedCall(
        const boost::optional<std::vector<PTR<OutArgument> > >& left_args_,
        const std::vector<PTR<OutArgument> >& right_args_,
        const boost::optional<std::vector<PTR<OutArgument> > >& scoped_args_);
    std::vector<PTR<OutArgument> > left_args;
    std::vector<PTR<OutArgument> > right_args;
    std::vector<PTR<OutArgument> > scoped_args;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct Field : public ValueModifier {
    Field(const Variable& variable_) : variable(variable_) {}
    Variable variable;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

  struct Index : public ValueModifier {
    Index(const std::vector<PTR<Expression> >& expressions_);
    std::vector<PTR<Expression> > expressions;
    std::string format() const;
    void accept(AstVisitor* visitor) { visitor->visit(this); }
  };

}}

#endif
