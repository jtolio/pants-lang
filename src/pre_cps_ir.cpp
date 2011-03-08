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
          found = true;
          opencall_idx = i;
        }
      }
      // okay, call term found, rewrite as a closed call
      PTR<ast::ClosedCall> call(new ast::ClosedCall);
      call->left_args.reserve(opencall_idx);
      for(unsigned int i = 0; i < opencall_idx; ++i) {
        std::vector<PTR<ast::Term> > terms;
        terms.push_back(app->terms[i]);
        PTR<ast::OutArgument> arg(new ast::RequiredOutArgument(
            PTR<ast::Expression>(new ast::Application(terms))));
        call->left_args.push_back(arg);
      }
      call->right_args.reserve(app->terms.size() - opencall_idx - 1);
      for(unsigned int i = opencall_idx + 1; i < app->terms.size(); ++i) {
        std::vector<PTR<ast::Term> > terms;
        terms.push_back(app->terms[i]);
        PTR<ast::OutArgument> arg(new ast::RequiredOutArgument(
            PTR<ast::Expression>(new ast::Application(terms))));
        call->right_args.push_back(arg);
      }
      PTR<ast::Term> term(new ast::Term(*app->terms[opencall_idx]));
      if(found) term->trailers.pop_back();
      term->trailers.push_back(call);
      std::vector<PTR<ast::Term> > terms;
      terms.push_back(term);
      ast::Application app(terms);
      
      // k, rewritten, try again.
      visit(&app);
      return;
    }
    // at this point all applications have just a single term.
    visit(app->terms[0].get());
  }
  
  void visit(ast::Term* term) {
    // if we got here, this is the term from a single-term application
    // process the term from the inside out, value first, trailers next.
    term->value->accept(this);
    for(unsigned int i = 0; i < term->trailers.size(); ++i) {
      term->trailers[i]->accept(this);
    }
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
    visit(index->expressions);
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
    PTR<pre_cps_ir::Term> call(new pre_cps_ir::Call(function));
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
  
  void visit(ast::Function* func) {
    std::cout << func->format() << std::endl;
  }
  
  void visit(ast::ClosedCall* call) {
    PTR<pre_cps_ir::Value> function = m_lastval;
    
    std::cout << call->format() << std::endl;
  }
  
  void visit(ast::Mutation* mut) { visit(mut, true); }
  void visit(ast::Definition* def) { visit(def, false); }
  void visit(ast::Assignment* assignment, bool mutation) {
    PTR<ast::Term> term(assignment->assignee->term);
    assignment->exp->accept(this);
    PTR<pre_cps_ir::Value> rhs = m_lastval;
    if(term->trailers.size() == 0) {
      ast::Variable* var = dynamic_cast<ast::Variable*>(term->value.get());
      if(!var)
        throw expectation_failure("left-hand side of an assignment must be a "
            "variable, field, or index");
      pre_cps_ir::Name name(*var);        
      PTR<pre_cps_ir::Assignee> assignee(new pre_cps_ir::SingleAssignee(name));
      m_ir->push_back(PTR<pre_cps_ir::Expression>(new pre_cps_ir::Assignment(
          assignee, rhs, mutation)));
    } else {
      throw expectation_failure("TODO: unimplemented");      
    }
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
