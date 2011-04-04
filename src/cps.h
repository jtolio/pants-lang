#ifndef __CPS_TRANSFORM_H__
#define __CPS_TRANSFORM_H__

#include "common.h"
#include "ir.h"

namespace cirth {
namespace cps {

  typedef cirth::ir::Name Name;
  struct Callable;

  struct Value {
    virtual ~Value() {}
    virtual std::string format(unsigned int indent_level) const = 0;
    protected: Value() {} };

  struct Field : public Value {
    Field(const Name& object_, const Name& field_)
      : object(object_), field(field_) {}
    std::string format(unsigned int indent_level) const;
    Name object;
    Name field;
  };

  struct Variable : public Value {
    Variable(const Name& variable_) : variable(variable_) {}
    Name variable;
    std::string format(unsigned int indent_level) const;
  };

  struct Integer : public Value {
    Integer(const long long& value_) : value(value_) {}
    long long value;
    std::string format(unsigned int indent_level) const;
  };

  struct CharString : public Value {
    CharString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format(unsigned int indent_level) const;
  };

  struct ByteString : public Value {
    ByteString(const std::string& value_) : value(value_) {}
    std::string value;
    std::string format(unsigned int indent_level) const;
  };

  struct Float : public Value {
    Float(const double& value_) : value(value_) {}
    double value;
    std::string format(unsigned int indent_level) const;
  };

  struct Definition {
    Definition(const Name& key_, const Name& value_)
      : key(key_), value(value_) {}
    Name key;
    Name value;
    std::string format(unsigned int indent_level) const;
  };

  struct Dictionary : public Value {
    std::vector<Definition> definitions;
    std::string format(unsigned int indent_level) const;
  };

  struct Array : public Value {
    std::vector<Name> values;
    std::string format(unsigned int indent_level) const;
  };

  struct Expression {
    virtual ~Expression() {}
    virtual std::string format(unsigned int indent_level) const = 0;
    virtual void callables(std::vector<PTR<Callable> >& callables) = 0;
    virtual void free_names(std::set<Name>& names) = 0;
    protected: Expression() {} };

  struct Call : public Expression {
    PTR<Value> callable;
    std::vector<Name> left_positional_args;
    boost::optional<Name> left_arbitrary_arg;
    std::vector<PTR<Value> > right_positional_args;
    std::vector<Definition> right_optional_args;
    boost::optional<Name> right_arbitrary_arg;
    boost::optional<Name> right_keyword_arg;
    std::vector<Definition> scoped_optional_args;
    boost::optional<Name> scoped_keyword_arg;
    PTR<Value> continuation;
    PTR<Value> exception;
    void callables(std::vector<PTR<Callable> >& callables);
    void free_names(std::set<Name>& names);
    std::string format(unsigned int indent_level) const;
  };

  struct VariableMutation : public Expression {
    VariableMutation(const Name& assignee_, Name value_,
        PTR<Expression> next_expression_)
      : assignee(assignee_), value(value_),
        next_expression(next_expression_) {}
    Name assignee;
    Name value;
    PTR<Expression> next_expression;
    void callables(std::vector<PTR<Callable> >& callables)
      { next_expression->callables(callables); }
    void free_names(std::set<Name>& names) {
      next_expression->free_names(names);
      names.insert(assignee);
      names.insert(value);
    }
    std::string format(unsigned int indent_level) const;
  };

  struct ObjectMutation : public Expression {
    ObjectMutation(const Name& object_, const Name& field_, const Name& value_,
        PTR<Expression> next_expression_)
      : object(object_), field(field_), value(value_),
        next_expression(next_expression_) {}
    Name object;
    Name field;
    Name value;
    PTR<Expression> next_expression;
    void callables(std::vector<PTR<Callable> >& callables)
      { next_expression->callables(callables); }
    void free_names(std::set<Name>& names) {
      next_expression->free_names(names);
      names.insert(object);
      names.insert(value);
    }
    std::string format(unsigned int indent_level) const;
  };

  struct Callable : public Value {
    PTR<Expression> expression;
    virtual void free_names(std::set<Name>& names) = 0;
    virtual void arg_names(std::set<Name>& names) = 0;
  };

  struct Function : public Callable {
    std::vector<Name> left_positional_args;
    boost::optional<Name> left_arbitrary_arg;
    std::vector<Name> right_positional_args;
    std::vector<Definition> right_optional_args;
    boost::optional<Name> right_arbitrary_arg;
    boost::optional<Name> right_keyword_arg;
    std::string format(unsigned int indent_level) const;
    void arg_names(std::set<Name>& names) {
      for(unsigned int i = 0; i < left_positional_args.size(); ++i)
        names.insert(left_positional_args[i]);
      if(!!left_arbitrary_arg) names.insert(left_arbitrary_arg.get());
      for(unsigned int i = 0; i < right_positional_args.size(); ++i)
        names.insert(right_positional_args[i]);
      for(unsigned int i = 0; i < right_optional_args.size(); ++i)
        names.insert(right_optional_args[i].key);
      if(!!right_arbitrary_arg) names.insert(right_arbitrary_arg.get());
      if(!!right_keyword_arg) names.insert(right_keyword_arg.get());
    }
    void free_names(std::set<Name>& names) {
      expression->free_names(names);
      std::set<Name> args;
      arg_names(args);
      for(std::set<Name>::iterator it(args.begin()); it != args.end(); ++it)
        names.erase(*it);
      for(unsigned int i = 0; i < right_optional_args.size(); ++i)
        names.insert(right_optional_args[i].value);
    }
  };

  struct Continuation : public Callable {
    std::vector<Name> vars;
    std::string format(unsigned int indent_level) const;
    void arg_names(std::set<Name>& names) {
      for(unsigned int i = 0; i < vars.size(); ++i)
        names.insert(vars[i]);
    }
    void free_names(std::set<Name>& names) {
      expression->free_names(names);
      for(unsigned int i = 0; i < vars.size(); ++i)
        names.erase(vars[i]);
    }
  };

  struct Scope : public Callable {
    std::string format(unsigned int indent_level) const;
    void arg_names(std::set<Name>& names) {}
    void free_names(std::set<Name>& names)
      { expression->free_names(names); }
  };

  void transform(const std::vector<PTR<cirth::ir::Expression> >& in_ir,
      const ir::Name& in_lastval, PTR<Expression>& out_ir);

}}

#endif
