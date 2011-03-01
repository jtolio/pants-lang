#include "pre_cps_ir.h"
#include <iostream>

using namespace cirth;

class ConversionVisitor : public ast::AstVisitor {
public:
  ConversionVisitor(std::vector<PTR<pre_cps_ir::Expression> >* ir) : m_ir(ir) {}

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
    if(term->trailers.size() > 0) {
      // process the term from the inside out, value first, trailers next.
      ast::Term new_term(*term);
      new_term.trailers.pop_back();
      visit(&new_term);
      term->trailers.back()->accept(this);
      return;
    }
    // this term has no trailers, so it's just a value
    term->value->accept(this);
  }
  
  void visit(ast::OpenCall*) {
    // if we got an open call at this point, then it was preempted by some
    // subsequent trailer. treat it as an argumentless closed call
    ast::ClosedCall call;
    visit(&call);
  }
  
  void visit(ast::ClosedCall* call) {
    std::cout << call->format() << std::endl;
  }
  
  void visit(ast::Field* field) {
    std::cout << field->format() << std::endl;
  }
  
  void visit(ast::Index* index) {
    std::cout << index->format() << std::endl;
  }
  
  void visit(ast::Variable* var) {
    std::cout << var->format() << std::endl;
  }
  
  void visit(ast::SubExpression* subexp) {
    std::cout << subexp->format() << std::endl;
  }
  
  void visit(ast::Integer* int_) {
    std::cout << int_->format() << std::endl;
  }
  
  void visit(ast::CharString* char_) {
    std::cout << char_->format() << std::endl;
  }
  
  void visit(ast::ByteString* byte) {
    std::cout << byte->format() << std::endl;
  }
  
  void visit(ast::Float* float_) {
    std::cout << float_->format() << std::endl;
  }
  
  void visit(ast::Dictionary* dict) {
    std::cout << dict->format() << std::endl;
  }
  
  void visit(ast::Array* array) {
    std::cout << array->format() << std::endl;
  }
  
  void visit(ast::Function* func) {
    std::cout << func->format() << std::endl;
  }

private:
  std::vector<PTR<pre_cps_ir::Expression> >* m_ir;
};

void pre_cps_ir::convert(const std::vector<PTR<ast::Expression> >& ast,
    std::vector<PTR<pre_cps_ir::Expression> >& ir) {
  ConversionVisitor visitor(&ir);
  for(unsigned int i = 0; i < ast.size(); ++i) {
    ast[i]->accept(&visitor);
  }
}
