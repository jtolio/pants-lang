#include "common.h"
#include "parser.h"
#include "wrap.h"
#include "ir.h"
#include "cps.h"
#include "compile.h"
#include <iostream>
#include "assets.h"

using namespace cirth;

void compile(const std::string& src) {

}

int main(int argc, char** argv) {
  std::string str;
  std::ostringstream os;
  os << assets::PRELUDE_CTH << "\n";

  while(getline(std::cin, str)) {
    os << str << '\n';
  }

  try {
    std::vector<PTR<ast::Expression> > ast;
    bool r = parser::parse(os.str(), ast);
    if(!r) throw expectation_failure("failed parsing!");

    std::vector<PTR<ir::Expression> > ir;
    wrap::ir_prepend(ir);
    ir::Name lastval(NULL_VALUE);
    ir::convert(ast, ir, lastval);
    ast.clear();

    PTR<cps::Expression> cps;
    cps::transform(ir, lastval, cps);
    ir.clear();

    compile::compile(cps, std::cout);
  } catch (const std::exception& e) {
    std::cerr << "failure: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
