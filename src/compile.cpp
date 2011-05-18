#include "compile.h"
#include "assets.h"
#include "wrap.h"

using namespace cirth::cps;

#define MIN_RIGHT_ARG_HIGHWATER 10
#define MIN_LEFT_ARG_HIGHWATER 2

static inline std::string scope_to_env(const std::string& scope) {
  std::ostringstream env;
  env << "((struct env_" << scope << "*)env)";
  return env.str();
}

static std::string var_access(const std::string& env, const Name& name) {
  std::ostringstream os;
  if(name.is_mutated())
    os << "(*" << env << "->" << name.c_name() << ".cell.addr)";
  else
    os << env << "->" << name.c_name();
  return os.str();
}

static std::string to_bytestring(const std::string& data) {
  std::ostringstream os;
  os << "\"" << std::hex;
  for(unsigned int i = 0; i < data.size(); ++i) os << "\\x" << (int)data[i];
  os << "\\x00\"";
  return os.str();
}

static void inline write_expression(PTR<Expression> cps, std::ostream& os,
    const std::string& scope);

static void inline write_closure(std::ostream& os, Callable* func,
    bool include_label, const std::string& name, const std::string& env) {
  os << "  " << name << ".t = CLOSURE;\n";
  if(include_label)
    os << "  " << name << ".closure.func = &&" << func->c_name() << ";\n";
  else
    os << "  " << name << ".closure.func = NULL;\n";
  std::set<Name> free_names;
  func->free_names(free_names);
  os << "  " << name << ".closure.env = GC_MALLOC(sizeof(struct env_"
        << func->c_name() << "));\n";
  for(std::set<Name>::const_iterator it(free_names.begin());
      it != free_names.end(); ++it) {
    os << "  ((struct env_" << func->c_name() << "*)" << name
       << ".closure.env)->" << it->c_name() << " = " << env << "->"
       << it->c_name() << ";\n";
  }
}

class ValueWriter : public ValueVisitor {
  public:
    ValueWriter(std::ostream* os, const std::string& env)
      : m_os(os), m_env(env) {}
    void visit(Field* field) {
      *m_os << "  dest = " << var_access(m_env, field->object) << ";\n"
               "  switch(dest.t) {\n"
               "    default:\n"
               "      THROW_ERROR("
            << var_access(m_env, HIDDEN_OBJECT) <<
               ", make_c_string(\"TODO: fields\"));\n"
               "    case OBJECT:\n"
               "      if(!get_field(dest.object.data, "
            << to_bytestring(field->field.c_name()) << ", "
            << field->field.c_name().size() << ", &dest)) {\n"
               "        THROW_ERROR("
            << var_access(m_env, HIDDEN_OBJECT) <<
               ", make_c_string(\"field %s not found!\", "
            << to_bytestring(field->field.c_name()) << "));\n"
               "      }\n"
               "      break;\n"
               "  }\n";
      m_lastval = "dest";
    }
    void visit(Variable* var) {
      m_lastval = var_access(m_env, var->variable);
    }
    void visit(Integer* integer) {
      *m_os << "  dest.t = INTEGER;\n"
               "  dest.integer.value = " << integer->value << ";\n";
      m_lastval = "dest";
    }
    void visit(String* str) {
      *m_os << "  dest.t = STRING;\n"
               "  dest.string.byte_oriented = "
            << (str->byte_oriented ? "true" : "false") << ";\n"
               "  dest.string.value = " << to_bytestring(str->value) << ";\n"
               "  dest.string.value_size = " << str->value.size() << ";\n";
      m_lastval = "dest";
    }
    void visit(Float* floating) {
      *m_os << "  dest.t = FLOAT;\n"
               "  dest.floating.value = " << floating->value << ";\n";
      m_lastval = "dest";
    }
    void visit(Callable* func) {
      write_closure(*m_os, func, true, "dest", m_env);
      m_lastval = "dest";
    }
    std::string lastval() const { return m_lastval; }
  private:
    std::ostream* m_os;
    std::string m_env;
    std::string m_lastval;
};

static void write_callable(std::ostream& os, Callable* func) {
  os << "\n" << func->c_name() << ":\n"
           "  MIN_RIGHT_ARGS(" << func->right_positional_args.size()
        << ")\n"
           "  MIN_LEFT_ARGS(" << func->left_positional_args.size()
        << ")\n";
  if(func->function) {
    os << "  " << scope_to_env(func->c_name()) << "->"
          << CONTINUATION.c_name() << " = continuation;\n"
             "  " << scope_to_env(func->c_name()) << "->"
          << HIDDEN_OBJECT.c_name() << " = hidden_object;\n";
  }
  for(unsigned int i = 0; i < func->right_positional_args.size(); ++i) {
    bool is_mutated(func->right_positional_args[i].is_mutated());
    os << "  " << scope_to_env(func->c_name()) << "->"
          << func->right_positional_args[i].c_name() << " = ";
    if(is_mutated) os << "make_cell(";
    os << "right_positional_args[" << i << "]";
    if(is_mutated) os << ")";
    os << ";\n";
  }
  for(unsigned int i = 0; i < func->left_positional_args.size(); ++i) {
    bool is_mutated(func->left_positional_args[i].is_mutated());
    os << "  " << scope_to_env(func->c_name()) << "->"
          << func->left_positional_args[i].c_name() << " = ";
    if(is_mutated) os << "make_cell(";
    os << "left_positional_args[" << i << "]";
    if(is_mutated) os << ")";
    os << ";\n";
  }
  write_expression(func->expression, os, func->c_name());
}

class ExpressionWriter : public ExpressionVisitor {
  public:
    ExpressionWriter(std::ostream* os, const std::string& env)
      : m_os(os), m_env(env) {}
    void visit(Call* call) {
      ValueWriter writer(m_os, m_env);

      if(call->continuation.get()) {
        call->continuation->accept(&writer);
        *m_os << "  continuation = " << writer.lastval() << ";\n";
      } else {
        *m_os << "  continuation.t = NIL;\n";
      }

      if(call->right_positional_args.size() > MIN_RIGHT_ARG_HIGHWATER) {
        *m_os << "  if(" << call->right_positional_args.size()
              << " > right_positional_args_highwater) {\n"
                 "    right_positional_args_highwater = "
              << call->right_positional_args.size() << ";\n"
                 "    right_positional_args = GC_MALLOC(sizeof(union Value) * "
              << call->right_positional_args.size() << ");\n"
                 "  }\n";
      }
      *m_os << "  right_positional_args_size = "
            << call->right_positional_args.size() << ";\n";

      for(unsigned int i = 0; i < call->right_positional_args.size(); ++i) {
        call->right_positional_args[i]->accept(&writer);
        *m_os << "  right_positional_args[" << i << "] = " << writer.lastval()
              << ";\n";
      }

      if(call->left_positional_args.size() > MIN_LEFT_ARG_HIGHWATER) {
        *m_os << "  if(" << call->left_positional_args.size()
              << " > left_positional_args_highwater) {\n"
                 "    left_positional_args_highwater = "
              << call->left_positional_args.size() << ";\n"
                 "    left_positional_args = GC_MALLOC(sizeof(union Value) * "
              << call->left_positional_args.size() << ");\n"
                 "  }\n";
      }
      *m_os << "  left_positional_args_size = "
            << call->left_positional_args.size() << ";\n";

      for(unsigned int i = 0; i < call->left_positional_args.size(); ++i) {
        *m_os << "  left_positional_args[" << i << "] = "
              << var_access(m_env, call->left_positional_args[i]) << ";\n";
      }

      if(call->hidden_object_optional_args.size() > 0) {
        *m_os << "  copy_object(&" << var_access(m_env, HIDDEN_OBJECT)
              << ", &hidden_object);\n";
        for(unsigned int i = 0; i < call->hidden_object_optional_args.size();
            ++i) {
          *m_os << "  set_field(hidden_object.object.data, "
                << to_bytestring(
                    call->hidden_object_optional_args[i].key.c_name())
                << ", "
                << call->hidden_object_optional_args[i].key.c_name().size()
                << ", &"
                << var_access(m_env, call->hidden_object_optional_args[i].value)
                << ");\n";
        }
        *m_os << "  seal_object(hidden_object.object.data);\n";
      } else {
        *m_os << "  hidden_object = " << var_access(m_env, HIDDEN_OBJECT)
              << ";\n";
      }
      Callable* callable(dynamic_cast<Callable*>(call->callable.get()));
      if(callable) {
        write_closure(*m_os, callable, false, "dest", m_env);
        *m_os << "  env = dest.closure.env;\n";
        write_callable(*m_os, callable);
        return;
      }
      call->callable->accept(&writer);
      if(writer.lastval() != "dest")
        *m_os << "  dest = " << writer.lastval() << ";\n";
      *m_os << "  if(dest.t != CLOSURE) {\n"
               "    THROW_ERROR(" << var_access(m_env, HIDDEN_OBJECT)
            << ", make_c_string(\"cannot call a non-function!\"));\n"
               "  }\n"
               "  CALL_FUNC(dest)\n";

    }
    void visit(VariableMutation* mut) {
      *m_os << "  (*" << m_env << "->" << mut->assignee.c_name()
            << ".cell.addr) = " << var_access(m_env, mut->value) << ";\n";
      mut->next_expression->accept(this);
    }
    void visit(ObjectMutation* mut) {
      *m_os << "  dest = " << var_access(m_env, mut->object) << ";\n"
               "  switch(dest.t) {\n"
               "    default:\n"
               "      THROW_ERROR(" << var_access(m_env, HIDDEN_OBJECT)
            << ", make_c_string(\"not an object!\"));\n"
               "    case OBJECT:\n"
               "      if(!set_field(dest.object.data, "
            << to_bytestring(mut->field.c_name()) << ", "
            << mut->field.c_name().size() << ", &"
            << var_access(m_env, mut->value) << ")) {\n"
               "        THROW_ERROR(" << var_access(m_env, HIDDEN_OBJECT)
            << ", make_c_string(\"object %s sealed!\", "
            << to_bytestring(mut->object.c_name()) << "));\n"
               "      }\n"
               "      break;\n"
               "  }\n";
      mut->next_expression->accept(this);
    }
  private:
    std::ostream* m_os;
    std::string m_env;
};

static void inline write_expression(PTR<Expression> cps, std::ostream& os,
    const std::string& scope) {
  ExpressionWriter writer(&os, scope_to_env(scope));
  cps->accept(&writer);
}

void cirth::compile::compile(PTR<Expression> cps, std::ostream& os) {

  std::vector<std::pair<PTR<cps::Callable>, bool> > callables;
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
  os << cirth::assets::DATA_STRUCTURES_C << "\n";
  os << cirth::assets::BUILTINS_C << "\n";

  for(unsigned int i = 0; i < callables.size(); ++i) {
    os << "struct env_" << callables[i].first->c_name() << " {\n";
    std::set<Name> free_names;
    std::set<Name> arg_names;
    callables[i].first->free_names(free_names);
    callables[i].first->arg_names(arg_names);
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

  for(unsigned int i = 0; i < callables.size(); ++i) {
    if(!callables[i].second) write_callable(os, callables[i].first.get());
  }

  os << cirth::assets::END_MAIN_C;

}
