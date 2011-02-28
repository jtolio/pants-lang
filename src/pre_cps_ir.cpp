#include "pre_cps_ir.h"
#include <iostream>

using namespace cirth;

class ConversionVisitor : public ast::AstVisitor {
public:
  ConversionVisitor(std::vector<PTR<pre_cps_ir::Expression> >* ir) : m_ir(ir) {}

  void visit(ast::Application* app) {
    if(app->terms.size() == 0)
      throw expectation_failure("application has 0 terms?!");
    if(app->terms.size() > 1) {
      // find call term
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
      std::cout << "opencall idx: " << opencall_idx << std::endl;
    }
  }
  void visit(ast::Mutation* mut) {
    std::cout << "mut" << std::endl;
  }
  void visit(ast::Definition* def) {
    std::cout << "def" << std::endl;
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
