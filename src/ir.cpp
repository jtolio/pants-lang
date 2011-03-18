#include "ir.h"

using namespace cirth;

class ConversionVisitor : public ast::AstVisitor {
public:
  ConversionVisitor(std::vector<PTR<ir::Expression> >* ir,
      unsigned long long* varcount)
    : m_ir(ir), m_varcount(varcount) {}

  void visit(const std::vector<PTR<ast::Expression> >& exps) {
    for(unsigned int i = 0; i < exps.size(); ++i)
      exps[i]->accept(this);
    m_ir->push_back(m_lastval);
  }

  void visit(ast::Application* app) {
    if(app->terms.size() <= 0)
      throw expectation_failure("application has 0 terms?!");
    if(app->terms.size() > 1) {
      // this is a multi-term application. find the call term
      unsigned int opencall_idx = 0;
      bool found = false;
      for(unsigned int i = 0; i < app->terms.size(); ++i) {
        ast::Term *term = app->terms[i].get();
        if(term->trailers.size() > 0 && dynamic_cast<ast::OpenCall*>(
            term->trailers[term->trailers.size() - 1].get())) {
          if(found)
            throw expectation_failure(
                "application has more than 1 open calls!");
          opencall_idx = i;
          found = true;
        }
      }
      // okay, call term found, process

      PTR<ir::Call> call(new ir::Call);

      call->left_positional_args.reserve(opencall_idx);
      for(unsigned int i = 0; i < opencall_idx; ++i) {
        app->terms[i]->accept(this);
        call->left_positional_args.push_back(ir::PositionalOutArgument(
            m_lastval));
      }

      ast::Term term(*app->terms[opencall_idx]);
      if(found) term.trailers.pop_back();
      visit(&term);
      call->function = m_lastval;

      call->right_positional_args.reserve(app->terms.size() - opencall_idx - 1);
      for(unsigned int i = opencall_idx + 1; i < app->terms.size(); ++i) {
        app->terms[i]->accept(this);
        call->right_positional_args.push_back(ir::PositionalOutArgument(
            m_lastval));
      }

      ir::Name name(gensym());
      PTR<ir::Assignee> assignee(new ir::SingleAssignee(name));
      m_ir->push_back(PTR<ir::Expression>(new ir::Assignment(
          assignee, call, false)));
      m_lastval = PTR<ir::Value>(new ir::Variable(name));

      return;
    }
    // at this point all applications have just a single term.
    visit(app->terms[0].get());
  }

  void visit(ast::Term* term) {
    // if we got here, this is the term from a single-term application
    // process the term from the inside out, value first, trailers next.
    term->value->accept(this);
    for(unsigned int i = 0; i < term->trailers.size(); ++i)
      term->trailers[i]->accept(this);
  }

  void visit(ast::OpenCall*) {
    // if we got an open call at this point, then it was preempted by some
    // subsequent trailer. treat it as an argumentless closed call
    ast::ClosedCall call;
    visit(&call);
  }

  void visit(ast::Field* field) {
    ir::Name name(gensym());
    PTR<ir::Assignee> assignee(new ir::SingleAssignee(name));
    PTR<ir::Term> term(new ir::Field(m_lastval,
        ir::Name(field->variable.name, field->variable.user_provided)));
    m_ir->push_back(PTR<ir::Expression>(new ir::Assignment(
        assignee, term, false)));
    m_lastval = PTR<ir::Value>(new ir::Variable(name));
  }

  void visit(ast::Index* index) {
    ir::Name name(gensym());
    PTR<ir::Assignee> assignee(new ir::SingleAssignee(name));
    PTR<ir::Value> array = m_lastval;
    ast::SubExpression subexp(index->expressions);
    visit(&subexp);
    PTR<ir::Term> term(new ir::Index(array, m_lastval));
    m_ir->push_back(PTR<ir::Expression>(new ir::Assignment(
        assignee, term, false)));
    m_lastval = PTR<ir::Value>(new ir::Variable(name));
  }

  void visit(ast::Variable* var) {
    m_lastval = PTR<ir::Value>(new ir::Variable(
        ir::Name(var->name, var->user_provided)));
  }

  void visit(ast::SubExpression* subexp) {
    PTR<ir::Function> function(new ir::Function(false));
    ConversionVisitor subvisitor(&function->expressions, m_varcount);
    subvisitor.visit(subexp->expressions);
    PTR<ir::Call> call(new ir::Call);
    call->function = function;
    ir::Name name(gensym());
    PTR<ir::Assignee> assignee(new ir::SingleAssignee(name));
    m_ir->push_back(PTR<ir::Expression>(new ir::Assignment(
        assignee, call, false)));
    m_lastval = PTR<ir::Value>(new ir::Variable(name));
  }

  void visit(ast::Integer* int_) {
    m_lastval = PTR<ir::Value>(new ir::Integer(int_->value));
  }

  void visit(ast::CharString* char_) {
    m_lastval = PTR<ir::Value>(new ir::CharString(
        char_->value));
  }

  void visit(ast::ByteString* byte) {
    m_lastval = PTR<ir::Value>(new ir::ByteString(byte->value));
  }

  void visit(ast::Float* float_) {
    m_lastval = PTR<ir::Value>(new ir::Float(float_->value));
  }

  void visit(ast::Dictionary* in_dict) {
    PTR<ir::Dictionary> out_dict(new ir::Dictionary);
    out_dict->definitions.reserve(in_dict->values.size());
    for(unsigned int i = 0; i < in_dict->values.size(); ++i) {
      in_dict->values[i].key->accept(this);
      PTR<ir::Value> key = m_lastval;
      in_dict->values[i].value->accept(this);
      PTR<ir::Value> value = m_lastval;
      out_dict->definitions.push_back(ir::DictDefinition(key, value));
    }
    m_lastval = out_dict;
  }

  void visit(ast::Array* in_array) {
    PTR<ir::Array> out_array(new ir::Array);
    out_array->values.reserve(in_array->values.size());
    for(unsigned int i = 0; i < in_array->values.size(); ++i) {
      in_array->values[i]->accept(this);
      out_array->values.push_back(m_lastval);
    }
    m_lastval = out_array;
  }

  void visit(ast::Mutation* mut) { visit(mut, true); }
  void visit(ast::Definition* def) { visit(def, false); }
  void visit(ast::Assignment* assignment, bool mutation) {
    PTR<ast::Term> term(assignment->assignee->term);
    PTR<ir::Assignee> assignee;
    PTR<ir::Value> rhs;

    if(mutation) {
      assignment->exp->accept(this);
      rhs = m_lastval;
    }

    if(term->trailers.size() == 0) {
      ast::Variable* var = dynamic_cast<ast::Variable*>(term->value.get());
      if(var)
        assignee.reset(new ir::SingleAssignee(ir::Name(*var)));
    } else {
      term->value->accept(this);
      for(unsigned int i = 0; i < term->trailers.size() - 1; ++i)
        term->trailers[i]->accept(this);
      ast::Field* field(dynamic_cast<ast::Field*>(term->trailers.back().get()));
      if(field) {
        assignee.reset(new ir::FieldAssignee(m_lastval,
            ir::Name(field->variable)));
      } else {
        ast::Index* index(dynamic_cast<ast::Index*>(
            term->trailers.back().get()));
        if(index) {
          PTR<ir::Value> array = m_lastval;
          ast::SubExpression subexp(index->expressions);
          visit(&subexp);
          assignee.reset(new ir::IndexAssignee(array, m_lastval));
        }
      }
    }

    if(!assignee)
      throw expectation_failure("left-hand side of an assignment must be a "
          "variable, field, or index");

    if(!mutation) {
      m_ir->push_back(PTR<ir::Expression>(new ir::Assignment(
          assignee, PTR<ir::Value>(new ir::Variable(
          ir::Name("null", false))), false)));
      assignment->exp->accept(this);
      rhs = m_lastval;
    }

    m_ir->push_back(PTR<ir::Expression>(new ir::Assignment(
        assignee, rhs, true)));
  }

  void visit(ast::Function* infunc) {
    PTR<ir::Function> outfunc(new ir::Function(true));
    outfunc->left_positional_args.reserve(infunc->left_required_args.size());
    for(unsigned int i = 0; i < infunc->left_required_args.size(); ++i) {
      outfunc->left_positional_args.push_back(ir::PositionalInArgument(
          ir::Name(infunc->left_required_args[i].name)));
    }
    if(!!infunc->left_arbitrary_arg)
      outfunc->left_arbitrary_arg = ir::ArbitraryInArgument(
          ir::Name(infunc->left_arbitrary_arg->name));
    outfunc->right_positional_args.reserve(infunc->right_required_args.size());
    for(unsigned int i = 0; i < infunc->right_required_args.size(); ++i) {
      outfunc->right_positional_args.push_back(ir::PositionalInArgument(
          ir::Name(infunc->right_required_args[i].name)));
    }
    outfunc->right_optional_args.reserve(infunc->right_optional_args.size());
    for(unsigned int i = 0; i < infunc->right_optional_args.size(); ++i) {
      infunc->right_optional_args[i].application->accept(this);
      outfunc->right_optional_args.push_back(ir::OptionalInArgument(
          ir::Name(infunc->right_optional_args[i].name), m_lastval));
    }
    if(!!infunc->right_arbitrary_arg)
      outfunc->right_arbitrary_arg = ir::ArbitraryInArgument(
          ir::Name(infunc->right_arbitrary_arg->name));
    if(!!infunc->right_keyword_arg)
      outfunc->right_keyword_arg = ir::KeywordInArgument(
          ir::Name(infunc->right_keyword_arg->name));

    ConversionVisitor subvisitor(&outfunc->expressions, m_varcount);
    subvisitor.visit(infunc->expressions);

    m_lastval = outfunc;
  }

  void visit(ast::ClosedCall* incall) {
    PTR<ir::Call> outcall(new ir::Call);
    outcall->function = m_lastval;
    if(!!incall->left_arbitrary_arg) {
      ast::SubExpression subexp(incall->left_arbitrary_arg.get().array);
      visit(&subexp);
      outcall->left_arbitrary_arg = ir::ArbitraryOutArgument(
          m_lastval);
    }
    outcall->left_positional_args.reserve(incall->left_required_args.size());
    for(unsigned int i = 0; i < incall->left_required_args.size(); ++i) {
      incall->left_required_args[i].application->accept(this);
      outcall->left_positional_args.push_back(
          ir::PositionalOutArgument(m_lastval));
    }
    outcall->right_positional_args.reserve(incall->right_required_args.size());
    for(unsigned int i = 0; i < incall->right_required_args.size(); ++i) {
      incall->right_required_args[i].application->accept(this);
      outcall->right_positional_args.push_back(
          ir::PositionalOutArgument(m_lastval));
    }
    outcall->right_optional_args.reserve(incall->right_optional_args.size());
    for(unsigned int i = 0; i < incall->right_optional_args.size(); ++i) {
      incall->right_optional_args[i].application->accept(this);
      outcall->right_optional_args.push_back(
          ir::OptionalOutArgument(ir::Name(
          incall->right_optional_args[i].name), m_lastval));
    }
    if(!!incall->right_arbitrary_arg) {
      ast::SubExpression subexp(incall->right_arbitrary_arg.get().array);
      visit(&subexp);
      outcall->right_arbitrary_arg = ir::ArbitraryOutArgument(
          m_lastval);
    }
    if(!!incall->right_keyword_arg) {
      ast::SubExpression subexp(incall->right_keyword_arg.get().object);
      visit(&subexp);
      outcall->right_keyword_arg = ir::KeywordOutArgument(m_lastval);
    }
    outcall->scoped_optional_args.reserve(incall->scoped_optional_args.size());
    for(unsigned int i = 0; i < incall->scoped_optional_args.size(); ++i) {
      incall->scoped_optional_args[i].application->accept(this);
      outcall->scoped_optional_args.push_back(
          ir::OptionalOutArgument(ir::Name(
          incall->scoped_optional_args[i].name), m_lastval));
    }
    if(!!incall->scoped_keyword_arg) {
      ast::SubExpression subexp(incall->scoped_keyword_arg.get().object);
      visit(&subexp);
      outcall->scoped_keyword_arg = ir::KeywordOutArgument(m_lastval);
    }
    ir::Name name(gensym());
    PTR<ir::Assignee> assignee(new ir::SingleAssignee(name));
    m_ir->push_back(PTR<ir::Expression>(new ir::Assignment(
        assignee, outcall, false)));
    m_lastval = PTR<ir::Value>(new ir::Variable(name));
  }

private:
  ir::Name gensym() {
    std::ostringstream os;
    os << "precpsir" << ++(*m_varcount);
    return ir::Name(os.str(), false);
  }

private:
  std::vector<PTR<ir::Expression> >* m_ir;
  PTR<ir::Value> m_lastval;
  unsigned long long* m_varcount;
};

void ir::convert(const std::vector<PTR<ast::Expression> >& ast,
    std::vector<PTR<ir::Expression> >& ir) {
  unsigned long long varcount = 0;
  ConversionVisitor visitor(&ir, &varcount);
  visitor.visit(ast);
}

std::string cirth::ir::Name::format() const {
  std::ostringstream os;
  os << "Name(" << name << (user_provided ?
      ", user_provided)" : ", compiler_provided)");
  return os.str();
}

std::string cirth::ir::Assignment::format() const {
  std::ostringstream os;
  os << "Assignment(" << assignee->format() << ", " << term->format() << ")";
  return os.str();
}

std::string cirth::ir::SingleAssignee::format() const {
  std::ostringstream os;
  os << "SingleAssignee(" << variable.format() << ")";
  return os.str();
}

std::string cirth::ir::IndexAssignee::format() const {
  std::ostringstream os;
  os << "IndexAssignee(" << array->format() << ", " << index->format() << ")";
  return os.str();
}

std::string cirth::ir::FieldAssignee::format() const {
  std::ostringstream os;
  os << "FieldAssignee(" << object->format() << ", " << field.format() << ")";
  return os.str();
}

std::string cirth::ir::Index::format() const {
  std::ostringstream os;
  os << "Index(" << array->format() << ", " << index->format() << ")";
  return os.str();
}

std::string cirth::ir::Field::format() const {
  std::ostringstream os;
  os << "Field(" << object->format() << ", " << field.format() << ")";
  return os.str();
}

std::string cirth::ir::Variable::format() const {
  std::ostringstream os;
  os << "Variable(" << variable.format() << ")";
  return os.str();
}

std::string cirth::ir::Integer::format() const {
  std::ostringstream os;
  os << "Integer(" << value << ")";
  return os.str();
}

std::string cirth::ir::CharString::format() const {
  std::ostringstream os;
  os << "CharString(" << value << ")";
  return os.str();
}

std::string cirth::ir::ByteString::format() const {
  std::ostringstream os;
  os << "ByteString(" << value << ")";
  return os.str();
}

std::string cirth::ir::Float::format() const {
  std::ostringstream os;
  os << "Float(" << value << ")";
  return os.str();
}

std::string cirth::ir::PositionalOutArgument::format() const {
  std::ostringstream os;
  os << "PositionalOutArgument(" << variable->format() << ")";
  return os.str();
}

std::string cirth::ir::OptionalOutArgument::format() const {
  std::ostringstream os;
  os << "OptionalOutArgument(" << key.format() << ", "
     << variable->format() << ")";
  return os.str();
}

std::string cirth::ir::ArbitraryOutArgument::format() const {
  std::ostringstream os;
  os << "ArbitraryOutArgument(" << variable->format() << ")";
  return os.str();
}

std::string cirth::ir::KeywordOutArgument::format() const {
  std::ostringstream os;
  os << "KeywordOutArgument(" << variable->format() << ")";
  return os.str();
}

std::string cirth::ir::DictDefinition::format() const {
  std::ostringstream os;
  os << "DictDefinition(" << key->format() << ", " << value->format() << ")";
  return os.str();
}

std::string cirth::ir::Dictionary::format() const {
  std::ostringstream os;
  os << "Dictionary(";
  for(unsigned int i = 0; i < definitions.size(); ++i) {
    if(i > 0) os << ", ";
    os << definitions[i].format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ir::Array::format() const {
  std::ostringstream os;
  os << "Array(";
  for(unsigned int i = 0; i < values.size(); ++i) {
    if(i > 0) os << ", ";
    os << values[i]->format();
  }
  os << ")";
  return os.str();
}

std::string cirth::ir::PositionalInArgument::format() const {
  std::ostringstream os;
  os << "PositionalInArgument(" << variable.format() << ")";
  return os.str();
}

std::string cirth::ir::OptionalInArgument::format() const {
  std::ostringstream os;
  os << "OptionalInArgument(" << variable.format() << ", "
     << defaultval->format() << ")";
  return os.str();
}

std::string cirth::ir::ArbitraryInArgument::format() const {
  std::ostringstream os;
  os << "ArbitraryInArgument(" << variable.format() << ")";
  return os.str();
}

std::string cirth::ir::KeywordInArgument::format() const {
  std::ostringstream os;
  os << "KeywordInArgument(" << variable.format() << ")";
  return os.str();
}

std::string cirth::ir::Call::format() const {
  std::ostringstream os;
  os << "Call(" << function->format() << ", Left(";
  bool comma_needed = false;
  for(unsigned int i = 0; i < left_positional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << left_positional_args[i].format();
    comma_needed = true;
  }
  if(!!left_arbitrary_arg) {
    if(comma_needed) os << ", ";
    os << left_arbitrary_arg.get().format();
  }
  comma_needed = false;
  os << "), Right(";
  for(unsigned int i = 0; i < right_positional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << right_positional_args[i].format();
    comma_needed = true;
  }
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << right_optional_args[i].format();
    comma_needed = true;
  }
  if(!!right_arbitrary_arg) {
    if(comma_needed) os << ", ";
    os << right_arbitrary_arg.get().format();
    comma_needed = true;
  }
  if(!!right_keyword_arg) {
    if(comma_needed) os << ", ";
    os << right_keyword_arg.get().format();
  }
  os << "), Scoped(";
  comma_needed = false;
  for(unsigned int i = 0; i < scoped_optional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << scoped_optional_args[i].format();
    comma_needed = true;
  }
  if(!!scoped_keyword_arg) {
    if(comma_needed) os << ", ";
    os << scoped_keyword_arg.get().format();
  }
  os << "))";
  return os.str();
}

std::string cirth::ir::Function::format() const {
  std::ostringstream os;
  os << "Function(" << (full_function ? "full_function, " :
      "partial_function, ");
  os << "Left(";
  bool comma_needed = false;
  for(unsigned int i = 0; i < left_positional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << left_positional_args[i].format();
    comma_needed = true;
  }
  if(!!left_arbitrary_arg) {
    if(comma_needed) os << ", ";
    os << left_arbitrary_arg.get().format();
  }
  os << "), Right(";
  comma_needed = false;
  for(unsigned int i = 0; i < right_positional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << right_positional_args[i].format();
    comma_needed = true;
  }
  for(unsigned int i = 0; i < right_optional_args.size(); ++i) {
    if(comma_needed) os << ", ";
    os << right_optional_args[i].format();
    comma_needed = true;
  }
  if(!!right_arbitrary_arg) {
    if(comma_needed) os << ", ";
    os << right_arbitrary_arg.get().format();
    comma_needed = true;
  }
  if(!!right_keyword_arg) {
    if(comma_needed) os << ", ";
    os << right_keyword_arg.get().format();
  }
  os << "), Expressions(";
  for(unsigned int i = 0; i < expressions.size(); ++i) {
    if(i > 0) os << ", ";
    os << expressions[i]->format();
  }
  os << "))";
  return os.str();
}
