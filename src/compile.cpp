#include "compile.h"
#include "assets.h"
#include "wrap.h"

using namespace cirth::cps;

static std::string var_access(const std::string& env, const Name& name) {
  std::ostringstream os;
  if(name.is_mutated())
    os << "(*" << env << "->" << name.c_name() << ".cell.addr)";
  else
    os << env << "->" << name.c_name();
  return os.str();
}

class ValueWriter : public ValueVisitor {
  public:
    ValueWriter(std::ostream* os, const std::string& env)
      : m_os(os), m_env(env) {}
    void visit(Field* field) {
      *m_os << "  printf(\"TODO: fields\\n\");\n"
               "  exit(1);\n";
    }
    void visit(Variable* var) {
      *m_os << "  dest = " << var_access(m_env, var->variable) << ";\n";
    }
    void visit(Integer* integer) {
      *m_os << "  dest.t = INTEGER;\n"
               "  dest.integer.value = " << integer->value << ";\n";
    }
    void visit(String*) {
      *m_os << "  printf(\"TODO: strings\\n\");\n"
               "  exit(1);\n";
    }
    void visit(Float* floating) {
      *m_os << "  dest.t = FLOAT;\n"
               "  dest.floating.value = " << floating->value << ";\n";
    }
    void visit(Function* func) { visit_callable(func); }
    void visit(Continuation* func) { visit_callable(func); }
    void visit(Scope* func) { visit_callable(func); }
    void visit_callable(Callable* func) {
      *m_os << "  dest.t = CLOSURE;\n"
               "  dest.closure.func = &&" << func->c_name() << ";\n";
      std::set<Name> free_names;
      func->free_names(free_names);
      *m_os << "  dest.closure.env = GC_MALLOC(sizeof(struct env_"
            << func->c_name() << "));\n";
      for(std::set<Name>::const_iterator it(free_names.begin());
          it != free_names.end(); ++it) {
        *m_os << "  ((struct env_" << func->c_name() << "*)dest.closure.env)->"
              << it->c_name() << " = " << m_env << "->" << it->c_name()
              << ";\n";
      }
    }
  private:
    std::ostream* m_os;
    std::string m_env;
};

class ExpressionWriter : public ExpressionVisitor {
  public:
    ExpressionWriter(std::ostream* os, const std::string& env)
      : m_os(os), m_env(env) {}
    void visit(Call* call) {
      ValueWriter writer(m_os, m_env);

      if(call->continuation.get()) {
        call->continuation->accept(&writer);
        *m_os << "  continuation = dest;\n";
      } else {
        *m_os << "  continuation.t = NIL;\n";
      }

      *m_os << "  right_positional_args = GC_MALLOC(sizeof(union Value) * "
            << (call->right_positional_args.size()) << ");\n";
      for(unsigned int i = 0; i < call->right_positional_args.size(); ++i) {
        call->right_positional_args[i]->accept(&writer);
        *m_os << "  right_positional_args[" << i << "] = dest;\n";
      }
      *m_os << "  right_positional_args_size = "
            << call->right_positional_args.size() << ";\n";

      *m_os << "  left_positional_args = GC_MALLOC(sizeof(union Value) * "
            << (call->left_positional_args.size()) << ");\n";
      for(unsigned int i = 0; i < call->left_positional_args.size(); ++i) {
        *m_os << "  left_positional_args[" << i << "] = "
              << var_access(m_env, call->left_positional_args[i]) << ";\n";
      }
      *m_os << "  left_positional_args_size = "
            << call->left_positional_args.size() << ";\n";

      call->callable->accept(&writer);
      *m_os << "  REQUIRED_FUNCTION(dest)\n"
               "  CALL_FUNC(dest)\n";
    }
    void visit(VariableMutation* mut) {
      *m_os << "  (*" << m_env << "->" << mut->assignee.c_name()
            << ".cell.addr) = " << var_access(m_env, mut->value) << ";\n";
      mut->next_expression->accept(this);
    }
    void visit(ObjectMutation* mut) {
      *m_os << "  printf(\"TODO: objects\\n\");\n"
               "  exit(1);\n";
      mut->next_expression->accept(this);
    }
  private:
    std::ostream* m_os;
    std::string m_env;
};

static inline std::string scope_to_env(const std::string& scope) {
  std::ostringstream env;
  env << "((struct env_" << scope << "*)env)";
  return env.str();
}

static void inline write_expression(PTR<Expression> cps, std::ostream& os,
    const std::string& scope) {
  ExpressionWriter writer(&os, scope_to_env(scope));
  cps->accept(&writer);
}

class CallableWriter : public ValueVisitor {
  public:
    CallableWriter(std::ostream* os) : m_os(os) {}
    void visit(Field*) {}
    void visit(Variable*) {}
    void visit(Integer*) {}
    void visit(String*) {}
    void visit(Float*) {}
    void visit(Function* func) {
      prelim(func);
      *m_os << "  REQUIRED_RIGHT_ARGS(" << func->right_positional_args.size()
            << ")\n";
      *m_os << "  REQUIRED_LEFT_ARGS(" << func->left_positional_args.size()
            << ")\n";
      for(unsigned int i = 0; i < func->right_positional_args.size(); ++i) {
        bool is_mutated(func->right_positional_args[i].is_mutated());
        *m_os << "  " << scope_to_env(func->c_name()) << "->"
              << func->right_positional_args[i].c_name() << " = ";
        if(is_mutated) *m_os << "make_cell(";
        *m_os << "right_positional_args[" << i << "]";
        if(is_mutated) *m_os << ")";
        *m_os << ";\n";
      }
      for(unsigned int i = 0; i < func->left_positional_args.size(); ++i) {
        bool is_mutated(func->left_positional_args[i].is_mutated());
        *m_os << "  " << scope_to_env(func->c_name()) << "->"
              << func->left_positional_args[i].c_name() << " = ";
        if(is_mutated) *m_os << "make_cell(";
        *m_os << "left_positional_args[" << i << "]";
        if(is_mutated) *m_os << ")";
        *m_os << ";\n";
      }
      *m_os << "  REQUIRED_FUNCTION(continuation)\n"
            "  " << scope_to_env(func->c_name()) << "->"
            << CONTINUATION.c_name() << " = continuation;\n";
      wrapup(func);
    }
    void visit(Continuation* func) {
      prelim(func);
      *m_os << "  REQUIRED_RIGHT_ARGS(" << func->vars.size() << ")\n";
      for(unsigned int i = 0; i < func->vars.size(); ++i) {
        bool is_mutated(func->vars[i].is_mutated());
        *m_os << "  " << scope_to_env(func->c_name()) << "->"
              << func->vars[i].c_name() << " = ";
        if(is_mutated) *m_os << "make_cell(";
        *m_os << "right_positional_args[" << i << "]";
        if(is_mutated) *m_os << ")";
        *m_os << ";\n";
      }
      wrapup(func);
    }
    void visit(Scope* func) {
      prelim(func);
      *m_os << "  REQUIRED_FUNCTION(continuation)\n"
            "  " << scope_to_env(func->c_name()) << "->"
            << CONTINUATION.c_name() << " = continuation;\n";
      wrapup(func);
    }
  private:
    void prelim(Callable* func) {
      *m_os << "\n" << func->c_name() << ":\n";
    }
    void wrapup(Callable* func) {
      write_expression(func->expression, *m_os, func->c_name());
    }
  private:
    std::ostream* m_os;
};

void cirth::compile::compile(PTR<Expression> cps, std::ostream& os) {

  std::vector<PTR<cps::Callable> > callables;
  std::set<Name> free_names;
  cps->callables(callables);
  cps->free_names(free_names);
  cirth::wrap::remove_provided_names(free_names);

  if(free_names.size() > 0) {
    std::ostringstream os;
    os << "unbound variable: " << free_names.begin()->format(0);
    throw expectation_failure(os.str());
  }

  os << cirth::assets::HEADER_C << "\n";

  for(unsigned int i = 0; i < callables.size(); ++i) {
    os << "struct env_" << callables[i]->c_name() << " {\n";
    std::set<Name> free_names;
    std::set<Name> arg_names;
    callables[i]->free_names(free_names);
    callables[i]->arg_names(arg_names);
    for(std::set<Name>::iterator it(free_names.begin()); it != free_names.end();
        ++it) {
      os << "  union Value " << it->c_name() << ";\n";
    }
    for(std::set<Name>::iterator it(arg_names.begin()); it != arg_names.end();
        ++it) {
      os << "  union Value " << it->c_name() << ";\n";
    }
    os << "};\n\n";
  }

  os << cirth::assets::START_MAIN_C;
  write_expression(cps, os, "main");

  CallableWriter writer(&os);
  for(unsigned int i = 0; i < callables.size(); ++i)
    callables[i]->accept(&writer);

  os << cirth::assets::END_MAIN_C;

}
