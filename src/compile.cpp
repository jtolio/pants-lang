#include "compile.h"
#include "assets.h"
#include "wrap.h"

using namespace pants::cps;

#define MIN_RIGHT_ARG_HIGHWATER 10
#define MIN_LEFT_ARG_HIGHWATER 10

class VariableContext {
public:
  VariableContext(unsigned int free_id, unsigned int frame_id)
    : m_freeID(free_id), m_frameID(frame_id) {}
  VariableContext(unsigned int free_id, unsigned int frame_id,
      const std::set<Name>& active_frame_names)
    : m_freeID(free_id), m_frameID(frame_id),
      m_activeFrameNames(active_frame_names) {}
  std::string varAccess(const Name& name) {
    std::ostringstream os;
    os << "((struct nameset_";
    if(m_activeFrameNames.find(name) != m_activeFrameNames.end()) {
      os << m_frameID << "*)frame)->";
    } else {
      os << m_freeID << "*)env)->";
    }
    os << name.c_name();
    return os.str();
  }
  std::string valAccess(const Name& name) {
    if(name.user_provided) {
      std::ostringstream os; // TODO
      os << "(*" << varAccess(name) << ".cell.addr)";
      return os.str();
    } else {
      return varAccess(name);
    }
  }
  void localDefinition(const Name& name) { m_activeFrameNames.insert(name); }
  unsigned int frameID() { return m_frameID; }
  unsigned int freeID() { return m_freeID; }
private:
  unsigned int m_freeID;
  unsigned int m_frameID;
  std::set<Name> m_activeFrameNames;
};

class NameSetManager {
public:
  typedef std::map<std::string, std::pair<std::set<Name>, unsigned int> >
          NameSetContainer;
public:
  void addSet(const std::set<Name>& names) { getID(names); }

  unsigned int getID(const std::set<Name>& names) {
    std::string key(namesetKey(names));
    NameSetContainer::iterator it(m_namesets.find(key));
    if(it != m_namesets.end()) return it->second.second;
    m_namesets[key].first = names;
    m_namesets[key].second = m_namesets.size();
    return m_namesets.size();
  }

  void writeStructs(std::ostream& os) {
    for(NameSetContainer::iterator it1(m_namesets.begin());
        it1 != m_namesets.end(); ++it1) {
      os << "struct nameset_" << it1->second.second << " {\n";
      for(std::set<Name>::iterator it2(it1->second.first.begin());
          it2 != it1->second.first.end(); ++it2) {
        os << "  union Value " << it2->c_name() << ";\n";
      }
      os << "};\n\n";
    }
  }

private:
  std::string namesetKey(const std::set<Name>& names) {
    std::ostringstream os;
    for(std::set<Name>::const_iterator it(names.begin()); it != names.end();
        ++it) {
      // yay sets being in sorted order always
      os << it->c_name() << ", ";
    }
    return os.str();
  }

private:
  NameSetContainer m_namesets;

};

static std::string to_bytestring(const std::string& data) {
  std::ostringstream os;
  os << "\"" << std::hex;
  for(unsigned int i = 0; i < data.size(); ++i) os << "\\x" << (int)data[i];
  os << "\\x00\"";
  return os.str();
}

static void inline write_expression(PTR<Expression> cps, std::ostream& os,
    VariableContext& context, NameSetManager& namesets);

class ValueWriter : public ValueVisitor {
  public:
    ValueWriter(std::ostream* os, VariableContext* context,
        NameSetManager* namesets)
      : m_os(os), m_context(context), m_namesets(namesets) {}
    void visit(Field* field) {
      *m_os << "  dest = " << m_context->valAccess(field->object) << ";\n"
               "  switch(dest.t) {\n"
               "    default:\n"
               "      THROW_ERROR("
            << m_context->valAccess(HIDDEN_OBJECT) <<
               ", make_c_string(\"TODO: fields\"));\n"
               "    case OBJECT:\n"
               "      if(!get_field(dest.object.data, (struct ByteArray){"
            << to_bytestring(field->field.c_name()) << ", "
            << field->field.c_name().size() << "}, &dest)) {\n"
               "        THROW_ERROR("
            << m_context->valAccess(HIDDEN_OBJECT) <<
               ", make_c_string(\"field %s not found!\", "
            << to_bytestring(field->field.c_name()) << "));\n"
               "      }\n"
               "      break;\n"
               "  }\n";
      m_lastval = "dest";
    }
    void visit(Variable* var) {
      m_lastval = m_context->valAccess(var->variable);
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
               "  dest.string.value.data = " << to_bytestring(str->value)
            << ";\n"
               "  dest.string.value.size = " << str->value.size() << ";\n";
      m_lastval = "dest";
    }
    void visit(Float* floating) {
      *m_os << "  dest.t = FLOAT;\n"
               "  dest.floating.value = " << floating->value << ";\n";
      m_lastval = "dest";
    }
    void visit(Callable* func) {
      std::set<Name> free_names;
      func->free_names(free_names);
      unsigned int free_id(m_namesets->getID(free_names));
      *m_os << "  dest.t = CLOSURE;\n"
               "  dest.closure.func = &&" << func->c_name() << ";\n";
      if(func->function) {
        *m_os << "  dest.closure.frame = NULL;\n"
                 "  dest.closure.env = GC_MALLOC(sizeof(struct nameset_"
              << free_id << "));\n";
        for(std::set<Name>::const_iterator it(free_names.begin());
            it != free_names.end(); ++it) {
          *m_os << "  ((struct nameset_" << free_id << "*)dest.closure.env)->"
                << it->c_name() << " = " << m_context->varAccess(*it) << ";\n";
        }
      } else {
        *m_os << "  dest.closure.frame = frame;\n"
                 "  dest.closure.env = env;\n";
      }
      m_lastval = "dest";
    }
    std::string lastval() const { return m_lastval; }
  private:
    std::ostream* m_os;
    VariableContext* m_context;
    NameSetManager* m_namesets;
    std::string m_lastval;
};

static void write_callable(std::ostream& os, Callable* func,
    VariableContext* context, NameSetManager* namesets) {
  // TODO: don't generate code we know we don't need!
  //   * don't generate keyword argument stuff for non-functions or functions
  //     with no right arguments.
  //   * don't deal with keyword arguments if none are passed in
  //   * don't require slot checking for arguments with default values.

  // set up callable's address
  os << "\n" << func->c_name() << ":\n";
  if(func->function) {
    // if it's actually a function, we want to save off the current
    // continuation, hidden object, and make a frame
    context->localDefinition(CONTINUATION);
    context->localDefinition(HIDDEN_OBJECT);
    os << "  frame = GC_MALLOC(sizeof(struct nameset_" << context->frameID()
       << "));\n"
          "  " << context->varAccess(CONTINUATION) << " = continuation;\n"
          "  " << context->varAccess(HIDDEN_OBJECT) << " = hidden_object;\n";
  }

  // were we given a right keyword argument? make space so we can add any
  // overflow keyword arguments if necessary
  if(!!func->right_keyword_arg) {
    bool is_mutated(func->right_keyword_arg->user_provided); // TODO
    context->localDefinition(*func->right_keyword_arg);
    os << "  make_object(&dest);\n"
          "  " << context->varAccess(*func->right_keyword_arg) << " = "
       << (is_mutated ? "make_cell(dest);\n" : "dest;\n");
  }

  // alright, go through and find all the names and positions of possible
  // keyword arguments. note that they only can live on the right side.
  std::map<Name, unsigned int> right_argument_slots;
  for(unsigned int i = 0; i < func->right_positional_args.size(); ++i) {
    if(right_argument_slots.find(func->right_positional_args[i]) !=
        right_argument_slots.end()) {
      // hopefully this has already been checked earlier in the compilation
      // stack. TODO: check and make sure
      throw pants::expectation_failure("argument name collision");
    }
    right_argument_slots[func->right_positional_args[i]] = i;
  }
  for(unsigned int i = 0; i < func->right_optional_args.size(); ++i) {
    if(right_argument_slots.find(func->right_optional_args[i].key) !=
        right_argument_slots.end()) {
      // hopefully this has already been checked earlier in the compilation
      // stack. TODO: check and make sure
      throw pants::expectation_failure("argument name collision");
    }
    right_argument_slots[func->right_optional_args[i].key] = i +
        func->right_positional_args.size();
  }

  // we're using a 64 bit unsigned integer to keep track of right argument
  // slots, so, we can't go over 64. TODO: actually make the limit 64 and not
  // 63.
  if(right_argument_slots.size() >= 64)
    throw pants::expectation_failure("too many right arguments");

  if(func->left_optional_args.size() > 0)
    throw pants::expectation_failure("TODO: left optional arguments");

  // if we have any possible named arguments, let's deal with them
  os << "  if(right_positional_args.size > " << right_argument_slots.size()
     << ") {\n"
        "    named_slots = " << ((1 << right_argument_slots.size()) - 1)
     << ";\n"
        "  } else {\n"
        "    named_slots = (1 << right_positional_args.size) - 1;\n"
        "  }\n"
        "  for(initialize_object_iterator(&it, &keyword_args);\n"
        "      !object_iterator_complete(&it);\n"
        "      object_iterator_step(&it)) {\n"
        "    j = " << right_argument_slots.size() << ";\n"
        "    i = binary_search(object_iterator_current_node(&it)->key,\n"
        "        (struct ByteArray[]){";
  for(std::map<Name, unsigned int>::iterator it(
      right_argument_slots.begin()); it != right_argument_slots.end();
      ++it) {
    if(it != right_argument_slots.begin()) os << ", ";
    os << "(struct ByteArray){" << to_bytestring(it->first.c_name())
       << ", " << it->first.c_name().size() << "}";
  }
  os << "}, " << right_argument_slots.size() << ");\n"
        "    switch(i) {\n"
        "      default: break;\n";
  {
    unsigned int i = 0;
    for(std::map<Name, unsigned int>::iterator it(
        right_argument_slots.begin()); it != right_argument_slots.end();
        ++it, ++i) {
      os << "      case " << i << ": j = " << it->second << "; break;\n";
    }
  }
  os << "    }\n"
        "    if(j == " << right_argument_slots.size() << ") {\n";
  if(!!func->right_keyword_arg) {
    os << "      set_field("
       << context->valAccess(*func->right_keyword_arg) << ".object.data, "
          "object_iterator_current_node(&it)->key, "
          "object_iterator_current_node(&it)->value);\n"
          "      continue;\n";
  } else {
    os << "      THROW_ERROR("
       << context->valAccess(HIDDEN_OBJECT) <<
          ", make_c_string(\"argument %s unknown!\", "
          "object_iterator_current_node(&it)->key.data));\n";
  }
  os << "    }\n"
        "    if(named_slots & (1 << j)) {\n"
        "      THROW_ERROR("
     << context->valAccess(HIDDEN_OBJECT) <<
        ", make_c_string(\"argument %s already provided!\", "
        "object_iterator_current_node(&it)->key.data));\n"
        "    }\n"
        "    named_slots |= (1 << j);\n"
        "    right_positional_args.data[j] = "
        "object_iterator_current_node(&it)->value;\n"
        "  }\n"
        "  initialize_object(&keyword_args);\n";

  // okay, now we have all of the provided and named arguments in slots, let's
  // throw in default values for anything still missing. we
  for(unsigned int i = 0; i < func->right_optional_args.size(); ++i) {
    os << "  if(!(named_slots & (1 << "
       << i + func->right_positional_args.size() << "))) {\n"
          "    right_positional_args.data["
       << i + func->right_positional_args.size() << "] = "
       << context->valAccess(func->right_optional_args[i].value) << ";\n"
       << "    named_slots |= (1 << "
       << i + func->right_positional_args.size() << ");\n"
       << "  }\n";
  }

  // let's seal the keyword object
  if(!!func->right_keyword_arg) {
    os << "  seal_object(" << context->valAccess(*func->right_keyword_arg)
       << ".object.data);\n";
  }

  // left arguments are easy to check if we got enough, let's check those.
  os << "  MIN_LEFT_ARGS(" << func->left_positional_args.size() << ")\n";

  // check right arguments, make sure we got everything.
  os << "  if((~named_slots) & ((1 << "
     << right_argument_slots.size() << ") - 1)) {\n"
        "    THROW_ERROR(" << context->valAccess(HIDDEN_OBJECT) << ", "
        "make_c_string(\"argument missing!\"));\n"
        "  }\n"
        "  if(right_positional_args.size < " << right_argument_slots.size()
     << ") {\n"
        "    right_positional_args.size = " << right_argument_slots.size()
     << ";\n"
        "  }\n";

  // okay, let's actually take our slots and fill in the real arguments
  for(unsigned int i = 0; i < func->right_positional_args.size(); ++i) {
    bool is_mutated(func->right_positional_args[i].user_provided); // TODO
    context->localDefinition(func->right_positional_args[i]);
    os << "  " << context->varAccess(func->right_positional_args[i]) << " = ";
    if(is_mutated) os << "make_cell(";
    os << "right_positional_args.data[" << i << "]";
    if(is_mutated) os << ")";
    os << ";\n";
  }
  for(unsigned int i = 0; i < func->right_optional_args.size(); ++i) {
    bool is_mutated(func->right_optional_args[i].key.user_provided); // TODO
    context->localDefinition(func->right_optional_args[i].key);
    os << "  " << context->varAccess(func->right_optional_args[i].key) << " = ";
    if(is_mutated) os << "make_cell(";
    os << "right_positional_args.data["
       << i + func->right_positional_args.size() << "]";
    if(is_mutated) os << ")";
    os << ";\n";
  }
  for(unsigned int i = 0; i < func->left_positional_args.size(); ++i) {
    bool is_mutated(func->left_positional_args[i].user_provided); // TODO
    context->localDefinition(func->left_positional_args[i]);
    os << "  " << context->varAccess(func->left_positional_args[i]) << " = ";
    if(is_mutated) os << "make_cell(";
    os << "left_positional_args.data[" << i << "]";
    if(is_mutated) os << ")";
    os << ";\n";
  }

  // okay, any overflow arguments go into an array
  if(!!func->right_arbitrary_arg) {
    bool is_mutated(func->right_arbitrary_arg->user_provided); // TODO
    context->localDefinition(*func->right_arbitrary_arg);
    os << "  make_array_object(&dest, (struct Array**)&raw_swap);\n"
          "  append_values(raw_swap, right_positional_args.data + "
       << right_argument_slots.size() << ", right_positional_args.size - "
       << right_argument_slots.size() << ");\n"
          "  " << context->varAccess(*func->right_arbitrary_arg) << " = "
       << (is_mutated ? "make_cell(dest);\n" : "dest;\n");
  } else {
    os << "  MAX_RIGHT_ARGS(" << right_argument_slots.size() << ")\n";
  }
  if(!!func->left_arbitrary_arg) {
    bool is_mutated(func->left_arbitrary_arg->user_provided); // TODO
    context->localDefinition(*func->left_arbitrary_arg);
    os << "  make_array_object(&dest, (struct Array**)&raw_swap);\n"
          "  append_values(raw_swap, left_positional_args.data + "
       << func->left_positional_args.size()
       << ", left_positional_args.size - "
       << func->left_positional_args.size() << ");\n"
          "  " << context->varAccess(*func->left_arbitrary_arg) << " = "
       << (is_mutated ? "make_cell(dest);\n" : "dest;\n");
  } else {
    os << "  MAX_LEFT_ARGS(" << func->left_positional_args.size() << ")\n";
  }

  // k, we should be set, let's run the function
  write_expression(func->expression, os, *context, *namesets);
}

class ExpressionWriter : public ExpressionVisitor {
  public:
    ExpressionWriter(std::ostream* os, VariableContext* context,
        NameSetManager* namesets)
      : m_os(os), m_context(context), m_namesets(namesets) {}
    void visit(Call* call) {
      ValueWriter writer(m_os, m_context, m_namesets);

      if(call->continuation.get()) {
        call->continuation->accept(&writer);
        *m_os << "  continuation = " << writer.lastval() << ";\n";
      } else {
        *m_os << "  continuation.t = NIL;\n";
      }

      if(!!call->right_keyword_arg) {
        *m_os << "  for(initialize_object_iterator(&it, "
              << m_context->valAccess(*call->right_keyword_arg)
              << ".object.data);\n"
                 "      !object_iterator_complete(&it);\n"
                 "      object_iterator_step(&it)) {\n"
                 "    set_field(&keyword_args,\n"
                 "              object_iterator_current_node(&it)->key,\n"
                 "              object_iterator_current_node(&it)->value);\n"
                 "  }\n";
      }
      if(call->right_optional_args.size() > 0) {
        for(unsigned int i = 0; i < call->right_optional_args.size(); ++i) {
          *m_os << "  set_field(&keyword_args, (struct ByteArray){"
                << to_bytestring(
                    call->right_optional_args[i].key.c_name())
                << ", "
                << call->right_optional_args[i].key.c_name().size()
                << "}, "
                << m_context->valAccess(call->right_optional_args[i].value)
                << ");\n";
        }
      }

      *m_os << "  i = 0;\n";
      if(call->right_positional_args.size() + call->right_optional_args.size() >
          MIN_RIGHT_ARG_HIGHWATER || !!call->right_arbitrary_arg ||
          !!call->right_keyword_arg) {
        if(!!call->right_arbitrary_arg) {
          *m_os << "  get_field("
                << m_context->valAccess(*call->right_arbitrary_arg)
                << ".object.data, (struct ByteArray){\"u_size\", 6}, &dest);\n";
          *m_os << "  i += ((struct Array*)dest.closure.env)->size;\n";
        }
        *m_os << "  right_positional_args.size = 0;\n"
                 "  reserve_space(&right_positional_args, "
              << call->right_positional_args.size() << " + i + "
                 "keyword_args.size);\n";
      }
      *m_os << "  right_positional_args.size = "
            << call->right_positional_args.size() << " + i;\n";

      for(unsigned int i = 0; i < call->right_positional_args.size(); ++i) {
        *m_os << "  right_positional_args.data[" << i << "] = "
              << m_context->valAccess(call->right_positional_args[i]) << ";\n";
      }

      if(!!call->right_arbitrary_arg) {
        *m_os << "  for(j = 0; j < i; ++j) {\n"
                 "    right_positional_args.data["
              << call->right_positional_args.size()
              << " + j] = ((struct Array*)dest.closure.env)->data[j];\n"
                 "  }\n";
      }

      *m_os << "  i = 0;\n";
      if(call->left_positional_args.size() > MIN_LEFT_ARG_HIGHWATER ||
          !!call->left_arbitrary_arg) {
        if(!!call->left_arbitrary_arg) {
          *m_os << "  get_field("
                << m_context->valAccess(*call->left_arbitrary_arg)
                << ".object.data, (struct ByteArray){\"u_size\", 6}, &dest);\n";
          *m_os << "  i = ((struct Array*)dest.closure.env)->size;\n";
        }
        *m_os << "  left_positional_args.size = 0;\n"
                 "  reserve_space(&left_positional_args, "
              << call->left_positional_args.size() << " + i);\n";
      }
      *m_os << "  left_positional_args.size = "
            << call->left_positional_args.size() << " + i;\n";

      for(unsigned int i = 0; i < call->left_positional_args.size(); ++i) {
        *m_os << "  left_positional_args.data[" << i << "] = "
              << m_context->valAccess(call->left_positional_args[i]) << ";\n";
      }

      if(!!call->left_arbitrary_arg) {
        *m_os << "  for(j = 0; j < i; ++j) {\n"
                 "    left_positional_args.data["
              << call->left_positional_args.size()
              << " + j] = ((struct Array*)dest.closure.env)->data[j];\n"
                 "  }\n";
      }

      if(call->hidden_object_optional_args.size() > 0) {
        *m_os << "  copy_object(&" << m_context->valAccess(HIDDEN_OBJECT)
              << ", &hidden_object);\n";
        for(unsigned int i = 0; i < call->hidden_object_optional_args.size();
            ++i) {
          *m_os << "  set_field(hidden_object.object.data, (struct ByteArray){"
                << to_bytestring(
                    call->hidden_object_optional_args[i].key.c_name())
                << ", "
                << call->hidden_object_optional_args[i].key.c_name().size()
                << "}, "
                << m_context->valAccess(
                    call->hidden_object_optional_args[i].value)
                << ");\n";
        }
        *m_os << "  seal_object(hidden_object.object.data);\n";
      } else {
        *m_os << "  hidden_object = " << m_context->valAccess(HIDDEN_OBJECT)
              << ";\n";
      }
      *m_os << "  dest = " << m_context->valAccess(call->callable) << ";\n"
               "  if(dest.t != CLOSURE) {\n"
               "    THROW_ERROR(" << m_context->valAccess(HIDDEN_OBJECT)
            << ", make_c_string(\"cannot call a non-function!\"));\n"
               "  }\n"
               "  CALL_FUNC(dest)\n";

      if(call->continuation.get())
        write_callable(*m_os, call->continuation.get(), m_context, m_namesets);
    }
    void visit(Assignment* assignment) {
      ValueWriter writer(m_os, m_context, m_namesets);
      assignment->value->accept(&writer);
      bool written = false;
      if(assignment->local) {
        bool is_mutated(assignment->assignee.user_provided); // TODO
        m_context->localDefinition(assignment->assignee);
        if(is_mutated) {
          *m_os << "  " << m_context->varAccess(assignment->assignee)
                << " = make_cell(" << writer.lastval() << ");\n";
          written = true;
        }
      }
      if(!written) {
        *m_os << "  " << m_context->valAccess(assignment->assignee) << " = "
              << writer.lastval() << ";\n";
      }
      assignment->next_expression->accept(this);
    }
    void visit(ObjectMutation* mut) {
      *m_os << "  dest = " << m_context->valAccess(mut->object) << ";\n"
               "  switch(dest.t) {\n"
               "    default:\n"
               "      THROW_ERROR(" << m_context->valAccess(HIDDEN_OBJECT)
            << ", make_c_string(\"not an object!\"));\n"
               "    case OBJECT:\n"
               "      if(!set_field(dest.object.data, (struct ByteArray){"
            << to_bytestring(mut->field.c_name()) << ", "
            << mut->field.c_name().size() << "}, "
            << m_context->valAccess(mut->value) << ")) {\n"
               "        THROW_ERROR(" << m_context->valAccess(HIDDEN_OBJECT)
            << ", make_c_string(\"object %s sealed!\", "
            << to_bytestring(mut->object.c_name()) << "));\n"
               "      }\n"
               "      break;\n"
               "  }\n";
      mut->next_expression->accept(this);
    }
  private:
    std::ostream* m_os;
    VariableContext* m_context;
    NameSetManager* m_namesets;
};

static void inline write_expression(PTR<Expression> cps, std::ostream& os,
    VariableContext& context, NameSetManager& namesets) {
  ExpressionWriter writer(&os, &context, &namesets);
  cps->accept(&writer);
}

void pants::compile::compile(PTR<Expression> cps, std::ostream& os,
    bool use_gc) {

  std::vector<PTR<cps::Callable> > callables;
  std::set<Name> free_names;
  cps->callables(callables);
  cps->free_names(free_names);

  std::set<Name> provided_names;
  pants::wrap::provided_names(provided_names);

  for(std::set<Name>::iterator it(provided_names.begin());
      it != provided_names.end(); ++it)
    free_names.erase(*it);

  if(free_names.size() > 0) {
    std::ostringstream os;
    os << "unbound variable: " << free_names.begin()->format(0);
    throw expectation_failure(os.str());
  }

  if(use_gc) os << "#define __USE_PANTS_GC\n";
  os << pants::assets::HEADER_C << "\n";
  os << pants::assets::DATA_STRUCTURES_C << "\n";
  os << pants::assets::BUILTINS_C << "\n";

  NameSetManager namesets;
  std::set<Name> names;
  // handle globals specially
  cps->free_names(names);
  cps->frame_names(names);
  namesets.addSet(names);
  VariableContext root_context(0, namesets.getID(names), provided_names);

  for(unsigned int i = 0; i < callables.size(); ++i) {
    if(!callables[i]->function) continue;
    names.clear();
    callables[i]->free_names(names);
    namesets.addSet(names);
    names.clear();
    callables[i]->frame_names(names);
    namesets.addSet(names);
  }

  namesets.writeStructs(os);

  os << pants::assets::START_MAIN_C;

  write_expression(cps, os, root_context, namesets);

  for(unsigned int i = 0; i < callables.size(); ++i) {
    if(callables[i]->function) {
      std::set<Name> free_names;
      callables[i]->free_names(free_names);
      std::set<Name> frame_names;
      callables[i]->frame_names(frame_names);
      VariableContext new_context(namesets.getID(free_names),
          namesets.getID(frame_names));
      write_callable(os, callables[i].get(), &new_context, &namesets);
    }
  }

  os << pants::assets::END_MAIN_C;

}
