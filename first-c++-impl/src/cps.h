#ifndef __CPS_TRANSFORM_H__
#define __CPS_TRANSFORM_H__

#include "common.h"
#include "ir.h"

namespace pants {
namespace cps {

  typedef pants::ir::Name Name;
  struct Callable; struct Field; struct VariableValue; struct Integer;
  struct String; struct Float; struct Callable; struct Call;
  struct Assignment; struct ObjectMutation;

  struct ValueVisitor {
    virtual void visit(Field*) = 0;
    virtual void visit(VariableValue*) = 0;
    virtual void visit(Integer*) = 0;
    virtual void visit(String*) = 0;
    virtual void visit(Float*) = 0;
    virtual void visit(Callable*) = 0;
  };

  struct ExpressionVisitor {
    virtual void visit(Call*) = 0;
    virtual void visit(Assignment*) = 0;
    virtual void visit(ObjectMutation*) = 0;
  };

  class Variable : boost::noncopyable {
  public:
    Variable(const Name& name_) : name(name_), m_varidSet(false) {}
    Name name;
    void setVarid(unsigned int varid) {
      m_varidSet = true;
      m_varid = varid;
    }
    unsigned int getVarid() const {
      if(!m_varidSet) throw expectation_failure("varid unset!");
      return m_varid;
    }
    std::string format(unsigned int indent_level = 0) const
      { return name.format(indent_level); }
  private:
    bool m_varidSet;
    unsigned int m_varid;
  };

  struct Value {
    virtual ~Value() {}
    virtual std::string format(unsigned int indent_level) const = 0;
    virtual void accept(ValueVisitor*) = 0;
    virtual void free_names(std::set<Name>& names) = 0;
    protected: Value() {} };

  struct Field : public Value {
    Field(PTR<Variable> object_, const Name& field_)
      : object(object_), field(field_) {}
    std::string format(unsigned int indent_level) const;
    void accept(ValueVisitor* v) { v->visit(this); }
    void free_names(std::set<Name>& names) { names.insert(object->name); }
    PTR<Variable> object;
    Name field;
  };

  struct VariableValue : public Value {
    VariableValue(PTR<Variable> variable_) : variable(variable_) {}
    PTR<Variable> variable;
    std::string format(unsigned int indent_level) const;
    void accept(ValueVisitor* v) { v->visit(this); }
    void free_names(std::set<Name>& names) { names.insert(variable->name); }
  };

  struct Integer : public Value {
    Integer(const long long& value_) : value(value_) {}
    long long value;
    std::string format(unsigned int indent_level) const;
    void accept(ValueVisitor* v) { v->visit(this); }
    void free_names(std::set<Name>& names) {}
  };

  struct String : public Value {
    String(const std::string& value_, bool byte_oriented_)
      : value(value_), byte_oriented(byte_oriented_) {}
    std::string value;
    bool byte_oriented;
    std::string format(unsigned int indent_level) const;
    void accept(ValueVisitor* v) { v->visit(this); }
    void free_names(std::set<Name>& names) {}
  };

  struct Float : public Value {
    Float(const double& value_) : value(value_) {}
    double value;
    std::string format(unsigned int indent_level) const;
    void accept(ValueVisitor* v) { v->visit(this); }
    void free_names(std::set<Name>& names) {}
  };

  struct InDefinition {
    InDefinition(PTR<Variable> key_, PTR<Variable> value_)
      : key(key_), value(value_) {}
    PTR<Variable> key;
    PTR<Variable> value;
    std::string format(unsigned int indent_level) const;
  };

  struct OutDefinition {
    OutDefinition(const Name& key_, PTR<Variable> value_)
      : key(key_), value(value_) {}
    Name key;
    PTR<Variable> value;
    std::string format(unsigned int indent_level) const;
  };

  struct Expression {
    virtual ~Expression() {}
    virtual std::string format(unsigned int indent_level) const = 0;
    virtual void callables(std::vector<PTR<Callable> >&) = 0;
    virtual void free_names(std::set<Name>& names) = 0;
    virtual void frame_names(std::set<Name>& names) = 0;
    virtual void accept(ExpressionVisitor*) = 0;
    protected: Expression() {} };

  struct Call : public Expression {
    Call(PTR<Variable> callable_) : callable(callable_) {}
    PTR<Variable> callable;
    std::vector<PTR<Variable> > left_positional_args;
    PTR<Variable> left_arbitrary_arg;
    std::vector<PTR<Variable> > right_positional_args;
    std::vector<OutDefinition> right_optional_args;
    PTR<Variable> right_arbitrary_arg;
    PTR<Variable> right_keyword_arg;
    PTR<Callable> continuation;
    void callables(std::vector<PTR<Callable> >& callables);
    void free_names(std::set<Name>& names);
    void frame_names(std::set<Name>& names);
    std::string format(unsigned int indent_level) const;
    void accept(ExpressionVisitor* v) { v->visit(this); }
  };

  struct Assignment : public Expression {
    Assignment(PTR<Variable> assignee_, PTR<Value> value_, bool local_,
        PTR<Expression> next_expression_)
      : assignee(assignee_), value(value_), local(local_),
        next_expression(next_expression_) {}
    PTR<Variable> assignee;
    PTR<Value> value;
    bool local;
    PTR<Expression> next_expression;
    void callables(std::vector<PTR<Callable> >& callables);
    void free_names(std::set<Name>& names);
    void frame_names(std::set<Name>& names) {
      if(local) names.insert(assignee->name);
      next_expression->frame_names(names);
    }
    std::string format(unsigned int indent_level) const;
    void accept(ExpressionVisitor* v) { v->visit(this); }
  };

  struct ObjectMutation : public Expression {
    ObjectMutation(PTR<Variable> object_, const Name& field_,
        PTR<Variable> value_, PTR<Expression> next_expression_)
      : object(object_), field(field_), value(value_),
        next_expression(next_expression_) {}
    PTR<Variable> object;
    Name field;
    PTR<Variable> value;
    PTR<Expression> next_expression;
    void callables(std::vector<PTR<Callable> >& callables)
      { next_expression->callables(callables); }
    void free_names(std::set<Name>& names) {
      next_expression->free_names(names);
      names.insert(object->name);
      names.insert(value->name);
    }
    void frame_names(std::set<Name>& names) {
      next_expression->frame_names(names);
    }
    std::string format(unsigned int indent_level) const;
    void accept(ExpressionVisitor* v) { v->visit(this); }
  };

  struct Callable : public Value {
    Callable(bool function_) : varid(m_varcount++), function(function_) {}
    PTR<Expression> expression;
    std::vector<PTR<Variable> > left_positional_args;
    std::vector<InDefinition> left_optional_args;
    PTR<Variable> left_arbitrary_arg;
    std::vector<PTR<Variable> > right_positional_args;
    std::vector<InDefinition> right_optional_args;
    PTR<Variable> right_arbitrary_arg;
    PTR<Variable> right_keyword_arg;
    unsigned int varid;
    bool function;
    std::string c_name() const {
      std::ostringstream os;
      os << "f_" << varid;
      return os.str();
    }
    std::string format(unsigned int indent_level) const;
    void accept(ValueVisitor* v) { v->visit(this); }
    void arg_names(std::set<Name>& names);
    void free_names(std::set<Name>& names);
    void frame_names(std::set<Name>& names) {
      arg_names(names);
      expression->frame_names(names);
    }
    private: static unsigned int m_varcount;
  };

  void transform(const std::vector<PTR<pants::ir::Expression> >& in_ir,
      const ir::Name& in_lastval, PTR<Expression>& out_ir);

}}

#endif
