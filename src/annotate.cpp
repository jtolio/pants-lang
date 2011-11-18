#include "annotate.h"
#include "wrap.h"

using namespace pants::cps;

class Scope;
void visit_expression(const Scope& scope, PTR<Expression> expression);

class Counter : boost::noncopyable {
  public:
    Counter() : m_counter(0) {}
    unsigned int inc() { return m_counter++; }
  private:
    unsigned int m_counter;
};

class Scope {
  public:
    Scope(Counter* counter) : m_counter(counter) {}
    unsigned int newVarid(const Name& name) {
      unsigned int id = m_counter->inc();
      m_vars[name] = id;
      return id;
    }
    unsigned int getVarid(const Name& name) {
      std::map<Name, unsigned int>::const_iterator it(m_vars.find(name));
      if(it == m_vars.end()) throw pants::expectation_failure("var missing");
      return it->second;
    }
  private:
    Counter* m_counter;
    std::map<Name, unsigned int> m_vars;
};

class VarIdValueVisitor : public ValueVisitor {
  public:
    VarIdValueVisitor(const Scope& scope) : m_scope(scope) {}

    void visit(Field* field) { getVar(field->object); }
    void visit(VariableValue* var) { getVar(var->variable); }
    void visit(Integer* integer) {}
    void visit(String* str) {}
    void visit(Float* floating) {}
    void visit(Callable* func) {
      for(unsigned int i = 0; i < func->left_optional_args.size(); ++i)
        getVar(func->left_optional_args[i].value);
      for(unsigned int i = 0; i < func->right_optional_args.size(); ++i)
        getVar(func->right_optional_args[i].value);
      for(unsigned int i = 0; i < func->left_positional_args.size(); ++i)
        newVar(func->left_positional_args[i]);
      for(unsigned int i = 0; i < func->left_optional_args.size(); ++i)
        newVar(func->left_optional_args[i].key);
      newVar(func->left_arbitrary_arg);
      for(unsigned int i = 0; i < func->right_positional_args.size(); ++i)
        newVar(func->right_positional_args[i]);
      for(unsigned int i = 0; i < func->right_optional_args.size(); ++i)
        newVar(func->right_optional_args[i].key);
      newVar(func->right_arbitrary_arg);
      newVar(func->right_keyword_arg);
      visit_expression(m_scope, func->expression);
    }

  private:
    Scope m_scope;

  protected:
    void getVar(PTR<Variable> var) {
      if(!var) return;
      var->setVarid(m_scope.getVarid(var->name));
    }
    void newVar(PTR<Variable> var) {
      if(!var) return;
      var->setVarid(m_scope.newVarid(var->name));
    }
};

class VarIdExpressionVisitor : public ExpressionVisitor {
  public:
    VarIdExpressionVisitor(const Scope& scope) : m_scope(scope) {}

    void visit(Call* call) {
      getVar(call->callable);
      for(unsigned int i = 0; i < call->left_positional_args.size(); ++i)
        getVar(call->left_positional_args[i]);
      getVar(call->left_arbitrary_arg);
      for(unsigned int i = 0; i < call->right_positional_args.size(); ++i)
        getVar(call->right_positional_args[i]);
      for(unsigned int i = 0; i < call->right_optional_args.size(); ++i)
        getVar(call->right_optional_args[i].value);
      getVar(call->right_arbitrary_arg);
      getVar(call->right_keyword_arg);
      if(call->continuation) {
        VarIdValueVisitor visitor(m_scope);
        call->continuation->accept(&visitor);
      }
    }
    void visit(Assignment* assignment) {
      VarIdValueVisitor visitor(m_scope);
      assignment->value->accept(&visitor);
      if(assignment->local) newVar(assignment->assignee);
      else getVar(assignment->assignee);
      assignment->next_expression->accept(this);
    }
    void visit(ObjectMutation* mut) {
      getVar(mut->object);
      getVar(mut->value);
      mut->next_expression->accept(this);
    }

  private:
    Scope m_scope;

  protected:
    void getVar(PTR<Variable> var) {
      if(!var) return;
      var->setVarid(m_scope.getVarid(var->name));
    }
    void newVar(PTR<Variable> var) {
      if(!var) return;
      var->setVarid(m_scope.newVarid(var->name));
    }
};

void visit_expression(const Scope& scope, PTR<Expression> expression) {
  VarIdExpressionVisitor visitor(scope);
  expression->accept(&visitor);
}

void pants::annotate::varids(PTR<Expression>& cps) {
  Counter id_counter;
  Scope root_scope(&id_counter);
  std::set<Name> provided_names;
  pants::wrap::provided_names(provided_names);
  for(std::set<Name>::iterator it(provided_names.begin());
      it != provided_names.end(); ++it)
    root_scope.newVarid(*it);
  visit_expression(root_scope, cps);
}
