#include "compile.h"
#include "constants.h"
#include "wrap.h"

using namespace cirth::cps;

class ValueWriter : public ValueVisitor {
  public:
    ValueWriter(std::ostream* os, const std::string& env)
      : m_os(os), m_env(env) {}
    void visit(Field* field) {
      *m_os << "\tprintf(\"TODO: fields\\n\");\n"
               "\texit(1);\n";
    }
    void visit(Variable* var) {
      *m_os << "\tdest = " << m_env << "->" << var->variable.c_name() << ";\n";
    }
    void visit(Integer* integer) {
      *m_os << "\tdest.t = INTEGER;\n"
               "\tdest.integer.value = " << integer->value << ";\n";
    }
    void visit(String*) {
      *m_os << "\tprintf(\"TODO: strings\\n\");\n"
               "\texit(1);\n";
    }
    void visit(Float* floating) {
      *m_os << "\tdest.t = FLOAT;\n"
               "\tdest.floating.value = " << floating->value << ";\n";
    }
    void visit(Function* func) { visit_callable(func); }
    void visit(Continuation* func) { visit_callable(func); }
    void visit(Scope* func) { visit_callable(func); }
    void visit_callable(Callable* func) {
      *m_os << "\tdest.t = CLOSURE;\n"
               "\tdest.closure.func = &&" << func->c_name() << ";\n"
               "\tdest.closure.env = alloc_env_" << func->c_name() << "(";
      std::set<Name> free_names;
      func->free_names(free_names);
      for(std::set<Name>::const_iterator it(free_names.begin());
          it != free_names.end(); ++it) {
        if(it != free_names.begin()) *m_os << ", ";
        *m_os << m_env << "->" << it->c_name();
      }
      *m_os << ");\n";
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
      *m_os << "\tright_positional_args = GC_MALLOC(sizeof(union Value) * "
            << (call->right_positional_args.size()) << ");\n";
      for(unsigned int i = 0; i < call->right_positional_args.size(); ++i) {
        call->right_positional_args[i]->accept(&writer);
        *m_os << "\tright_positional_args[" << i << "] = dest;\n";
      }
      *m_os << "\tright_positional_args_size = "
            << call->right_positional_args.size() << ";\n";
      if(call->continuation.get()) {
        call->continuation->accept(&writer);
        *m_os << "\tcontinuation = dest;\n";
      } else {
        *m_os << "\tcontinuation.t = NIL;\n";
      }
      call->callable->accept(&writer);
      *m_os << "\tif(dest.t != CLOSURE) {\n"
               "\t\tprintf(\"cannot call a non-function!\\n\");\n"
               "\t\texit(1);\n"
               "\t}\n"
               "\tenv = dest.closure.env;\n"
               "\tgoto *dest.closure.func;\n";
    }
    void visit(VariableMutation* mut) {
      *m_os << "\t" << m_env << "->" << mut->assignee.c_name() << " = " << m_env
            << "->" << mut->value.c_name() << ";\n";
      mut->next_expression->accept(this);
    }
    void visit(ObjectMutation* mut) {
      *m_os << "\tprintf(\"TODO: objects\\n\");\n"
               "\texit(1);\n";
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
      *m_os << "\tif(right_positional_args_size < "
            << func->right_positional_args.size() << ") {\n"
            "\t\tprintf(\"function takes "
            << func->right_positional_args.size()
            << " arguments, %d given.\\n\", right_positional_args_size);\n"
            "\t\texit(1);\n"
            "\t}\n";
      for(unsigned int i = 0; i < func->right_positional_args.size(); ++i) {
        *m_os << "\t" << scope_to_env(func->c_name()) << "->"
              << func->right_positional_args[i].c_name()
              << " = right_positional_args[" << i << "];\n";
      }
      *m_os << "\tif(continuation.t != CLOSURE) {\n"
            "\t\tprintf(\"function requires continuation, none given.\\n\");\n"
            "\t\texit(1);\n"
            "\t}\n"
            "\t" << scope_to_env(func->c_name()) << "->"
            << CONTINUATION.c_name() << " = continuation;\n";
      wrapup(func);
    }
    void visit(Continuation* func) {
      prelim(func);
      *m_os << "\tif(right_positional_args_size < "
            << func->vars.size() << ") {\n"
            "\t\tprintf(\"function takes " << func->vars.size()
            << " arguments, %d given.\\n\", right_positional_args_size);\n"
            "\t\texit(1);\n"
            "\t}\n";
      for(unsigned int i = 0; i < func->vars.size(); ++i) {
        *m_os << "\t" << scope_to_env(func->c_name()) << "->"
              << func->vars[i].c_name()
              << " = right_positional_args[" << i << "];\n";
      }
      wrapup(func);
    }
    void visit(Scope* func) {
      prelim(func);
      *m_os << "\tif(continuation.t != CLOSURE) {\n"
            "\t\tprintf(\"function requires continuation, none given.\\n\");\n"
            "\t\texit(1);\n"
            "\t}\n"
            "\t" << scope_to_env(func->c_name()) << "->"
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

  os << HEADER << "\n";

  for(unsigned int i = 0; i < callables.size(); ++i) {
    os << "struct env_" << callables[i]->c_name() << " {\n";
    std::set<Name> free_names;
    std::set<Name> arg_names;
    callables[i]->free_names(free_names);
    callables[i]->arg_names(arg_names);
    for(std::set<Name>::iterator it(free_names.begin()); it != free_names.end();
        ++it) {
      os << "\tunion Value " << it->c_name() << ";\n";
    }
    for(std::set<Name>::iterator it(arg_names.begin()); it != arg_names.end();
        ++it) {
      os << "\tunion Value " << it->c_name() << ";\n";
    }
    os << "};\n\n";

    os << "struct env_" << callables[i]->c_name() << "* alloc_env_"
       << callables[i]->c_name() << "(";
    for(std::set<Name>::iterator it(free_names.begin()); it != free_names.end();
        ++it) {
      if(it != free_names.begin()) os << ", ";
      os << "union Value " << it->c_name();
    }
    os << ") {\n\tstruct env_" << callables[i]->c_name()
       << "* env = GC_MALLOC(sizeof(struct env_" << callables[i]->c_name()
       << "));\n";
    for(std::set<Name>::iterator it(free_names.begin()); it != free_names.end();
        ++it) {
      os << "\tenv->" << it->c_name() << " = " << it->c_name() << ";\n";
    }
    os << "\treturn env;\n}\n\n";
  }

  os << STARTMAIN;
  write_expression(cps, os, "main");

  CallableWriter writer(&os);
  for(unsigned int i = 0; i < callables.size(); ++i)
    callables[i]->accept(&writer);

  os << "halt:\n";
  os << "\treturn 0;\n";
  os << "}\n";

}
