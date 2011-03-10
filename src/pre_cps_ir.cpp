#include "pre_cps_ir.h"
#include <iostream>

using namespace cirth;

class ConversionVisitor : public ast::AstVisitor {
public:
  ConversionVisitor(std::vector<PTR<pre_cps_ir::Expression> >* ir,
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
      
      PTR<pre_cps_ir::Call> call(new pre_cps_ir::Call);
      
      call->left_positional_args.reserve(opencall_idx);
      for(unsigned int i = 0; i < opencall_idx; ++i) {
        app->terms[i]->accept(this);
        call->left_positional_args.push_back(pre_cps_ir::PositionalOutArgument(
            m_lastval));
      }
      
      ast::Term term(*app->terms[opencall_idx]);
      if(found) term.trailers.pop_back();
      visit(&term);
      call->function = m_lastval;
      
      call->right_positional_args.reserve(app->terms.size() - opencall_idx - 1);
      for(unsigned int i = opencall_idx + 1; i < app->terms.size(); ++i) {
        app->terms[i]->accept(this);
        call->right_positional_args.push_back(pre_cps_ir::PositionalOutArgument(
            m_lastval));
      }
      
      pre_cps_ir::Name name(gensym());
      PTR<pre_cps_ir::Assignee> assignee(new pre_cps_ir::SingleAssignee(name));
      m_ir->push_back(PTR<pre_cps_ir::Expression>(new pre_cps_ir::Assignment(
          assignee, call, false)));
      m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::Variable(name));

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
    pre_cps_ir::Name name(gensym());
    PTR<pre_cps_ir::Assignee> assignee(new pre_cps_ir::SingleAssignee(name));
    PTR<pre_cps_ir::Term> term(new pre_cps_ir::Field(m_lastval,
        pre_cps_ir::Name(field->variable.name, field->variable.user_provided)));
    m_ir->push_back(PTR<pre_cps_ir::Expression>(new pre_cps_ir::Assignment(
        assignee, term, false)));
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::Variable(name));
  }
  
  void visit(ast::Index* index) {
    pre_cps_ir::Name name(gensym());
    PTR<pre_cps_ir::Assignee> assignee(new pre_cps_ir::SingleAssignee(name));
    PTR<pre_cps_ir::Value> array = m_lastval;
    ast::SubExpression subexp(index->expressions);
    visit(&subexp);
    PTR<pre_cps_ir::Term> term(new pre_cps_ir::Index(array, m_lastval));
    m_ir->push_back(PTR<pre_cps_ir::Expression>(new pre_cps_ir::Assignment(
        assignee, term, false)));
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::Variable(name));
  }
  
  void visit(ast::Variable* var) {
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::Variable(
        pre_cps_ir::Name(var->name, var->user_provided)));
  }
  
  void visit(ast::SubExpression* subexp) {
    PTR<pre_cps_ir::Function> function(new pre_cps_ir::Function(false));
    ConversionVisitor subvisitor(&function->expressions, m_varcount);
    subvisitor.visit(subexp->expressions);
    PTR<pre_cps_ir::Call> call(new pre_cps_ir::Call);
    call->function = function;
    pre_cps_ir::Name name(gensym());
    PTR<pre_cps_ir::Assignee> assignee(new pre_cps_ir::SingleAssignee(name));
    m_ir->push_back(PTR<pre_cps_ir::Expression>(new pre_cps_ir::Assignment(
        assignee, call, false)));
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::Variable(name));
  }
  
  void visit(ast::Integer* int_) {
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::Integer(int_->value));
  }
  
  void visit(ast::CharString* char_) {
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::CharString(char_->value));
  }
  
  void visit(ast::ByteString* byte) {
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::ByteString(byte->value));
  }
  
  void visit(ast::Float* float_) {
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::Float(float_->value));
  }
  
  void visit(ast::Dictionary* in_dict) {
    PTR<pre_cps_ir::Dictionary> out_dict(new pre_cps_ir::Dictionary);
    out_dict->definitions.reserve(in_dict->values.size());
    for(unsigned int i = 0; i < in_dict->values.size(); ++i) {
      in_dict->values[i].key->accept(this);
      PTR<pre_cps_ir::Value> key = m_lastval;
      in_dict->values[i].value->accept(this);
      PTR<pre_cps_ir::Value> value = m_lastval;
      out_dict->definitions.push_back(pre_cps_ir::DictDefinition(key, value));
    }
    m_lastval = out_dict;
  }
  
  void visit(ast::Array* in_array) {
    PTR<pre_cps_ir::Array> out_array(new pre_cps_ir::Array);
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
    PTR<pre_cps_ir::Assignee> assignee;
    PTR<pre_cps_ir::Value> rhs;
    
    if(mutation) {
      assignment->exp->accept(this);
      rhs = m_lastval;
    }
    
    if(term->trailers.size() == 0) {
      ast::Variable* var = dynamic_cast<ast::Variable*>(term->value.get());
      if(var)
        assignee.reset(new pre_cps_ir::SingleAssignee(pre_cps_ir::Name(*var)));
    } else {
      term->value->accept(this);
      for(unsigned int i = 0; i < term->trailers.size() - 1; ++i)
        term->trailers[i]->accept(this);
      ast::Field* field(dynamic_cast<ast::Field*>(term->trailers.back().get()));
      if(field) {
        assignee.reset(new pre_cps_ir::FieldAssignee(m_lastval,
            pre_cps_ir::Name(field->variable)));
      } else {
        ast::Index* index(dynamic_cast<ast::Index*>(
            term->trailers.back().get()));
        if(index) {
          PTR<pre_cps_ir::Value> array = m_lastval;
          ast::SubExpression subexp(index->expressions);
          visit(&subexp);
          assignee.reset(new pre_cps_ir::IndexAssignee(array, m_lastval));
        }
      }
    }
    
    if(!assignee)
      throw expectation_failure("left-hand side of an assignment must be a "
          "variable, field, or index");
    
    if(!mutation) {
      m_ir->push_back(PTR<pre_cps_ir::Expression>(new pre_cps_ir::Assignment(
          assignee, PTR<pre_cps_ir::Value>(new pre_cps_ir::Variable(
          pre_cps_ir::Name("null", false))), false)));
      assignment->exp->accept(this);
      rhs = m_lastval;
    }

    m_ir->push_back(PTR<pre_cps_ir::Expression>(new pre_cps_ir::Assignment(
        assignee, rhs, true)));
  }
  
  void visit(ast::Function* infunc) {
    PTR<pre_cps_ir::Function> outfunc(new pre_cps_ir::Function(true));
    outfunc->left_positional_args.reserve(infunc->left_required_args.size());
    for(unsigned int i = 0; i < infunc->left_required_args.size(); ++i) {
      outfunc->left_positional_args.push_back(pre_cps_ir::PositionalInArgument(
          pre_cps_ir::Name(infunc->left_required_args[i].name)));
    }
    outfunc->right_positional_args.reserve(infunc->right_required_args.size());
    for(unsigned int i = 0; i < infunc->right_required_args.size(); ++i) {
      outfunc->right_positional_args.push_back(pre_cps_ir::PositionalInArgument(
          pre_cps_ir::Name(infunc->right_required_args[i].name)));
    }
    outfunc->right_optional_args.reserve(infunc->right_optional_args.size());
    for(unsigned int i = 0; i < infunc->right_optional_args.size(); ++i) {
      infunc->right_optional_args[i].application->accept(this);
      outfunc->right_optional_args.push_back(pre_cps_ir::OptionalInArgument(
          pre_cps_ir::Name(infunc->right_optional_args[i].name), m_lastval));
    }
    if(!!infunc->right_arbitrary_arg)
      outfunc->right_arbitrary_arg = pre_cps_ir::ArbitraryInArgument(
          pre_cps_ir::Name(infunc->right_arbitrary_arg->name));
    if(!!infunc->right_keyword_arg)
      outfunc->right_keyword_arg = pre_cps_ir::KeywordInArgument(
          pre_cps_ir::Name(infunc->right_keyword_arg->name));

    ConversionVisitor subvisitor(&outfunc->expressions, m_varcount);
    subvisitor.visit(infunc->expressions);

    m_lastval = outfunc;
  }
  
  void visit(ast::ClosedCall* incall) {
    PTR<pre_cps_ir::Call> outcall(new pre_cps_ir::Call);
    outcall->function = m_lastval;
    outcall->left_positional_args.reserve(incall->left_required_args.size());
    for(unsigned int i = 0; i < incall->left_required_args.size(); ++i) {
      incall->left_required_args[i].application->accept(this);
      outcall->left_positional_args.push_back(
          pre_cps_ir::PositionalOutArgument(m_lastval));
    }
    outcall->right_positional_args.reserve(incall->right_required_args.size());
    for(unsigned int i = 0; i < incall->right_required_args.size(); ++i) {
      incall->right_required_args[i].application->accept(this);
      outcall->right_positional_args.push_back(
          pre_cps_ir::PositionalOutArgument(m_lastval));
    }
    outcall->right_optional_args.reserve(incall->right_optional_args.size());
    for(unsigned int i = 0; i < incall->right_optional_args.size(); ++i) {
      incall->right_optional_args[i].application->accept(this);
      outcall->right_optional_args.push_back(
          pre_cps_ir::OptionalOutArgument(pre_cps_ir::Name(
          incall->right_optional_args[i].name), m_lastval));
    }
    if(!!incall->right_arbitrary_arg) {
      ast::SubExpression subexp(incall->right_arbitrary_arg.get().array);
      visit(&subexp);
      outcall->right_arbitrary_arg = pre_cps_ir::ArbitraryOutArgument(
          m_lastval);
    }
    if(!!incall->right_keyword_arg) {
      ast::SubExpression subexp(incall->right_keyword_arg.get().object);
      visit(&subexp);
      outcall->right_keyword_arg = pre_cps_ir::KeywordOutArgument(m_lastval);
    }
    outcall->scoped_optional_args.reserve(incall->scoped_optional_args.size());
    for(unsigned int i = 0; i < incall->scoped_optional_args.size(); ++i) {
      incall->scoped_optional_args[i].application->accept(this);
      outcall->scoped_optional_args.push_back(
          pre_cps_ir::OptionalOutArgument(pre_cps_ir::Name(
          incall->scoped_optional_args[i].name), m_lastval));
    }
    if(!!incall->scoped_keyword_arg) {
      ast::SubExpression subexp(incall->scoped_keyword_arg.get().object);
      visit(&subexp);
      outcall->scoped_keyword_arg = pre_cps_ir::KeywordOutArgument(m_lastval);
    }
    pre_cps_ir::Name name(gensym());
    PTR<pre_cps_ir::Assignee> assignee(new pre_cps_ir::SingleAssignee(name));
    m_ir->push_back(PTR<pre_cps_ir::Expression>(new pre_cps_ir::Assignment(
        assignee, outcall, false)));
    m_lastval = PTR<pre_cps_ir::Value>(new pre_cps_ir::Variable(name));
  }
  
private:
  pre_cps_ir::Name gensym() {
    std::ostringstream os;
    os << "precpsir" << ++(*m_varcount);
    return pre_cps_ir::Name(os.str(), false);
  }

private:
  std::vector<PTR<pre_cps_ir::Expression> >* m_ir;
  PTR<pre_cps_ir::Value> m_lastval;
  unsigned long long* m_varcount;
};

void pre_cps_ir::convert(const std::vector<PTR<ast::Expression> >& ast,
    std::vector<PTR<pre_cps_ir::Expression> >& ir) {
  unsigned long long varcount = 0;
  ConversionVisitor visitor(&ir, &varcount);
  visitor.visit(ast);
}
