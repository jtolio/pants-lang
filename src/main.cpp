#include "common.h"
#include "parser.h"
#include "wrap.h"
#include "ir.h"
#include "cps.h"
#include "compile.h"
#include <iostream>
#include "assets.h"

using namespace cirth;

int main(int argc, char** argv) {
  std::string str;
  std::ostringstream os;
  os << assets::PRELUDE_CTH << "\n";

  while(getline(std::cin, str)) {
    os << str << '\n';
  }

  try {
    // force counters
    UPDATE_FIELD; // gets 0
    LOOKUP_FIELD; // gets 1

    std::vector<PTR<ast::Expression> > ast;
    bool r = parser::parse(os.str(), ast);
    if(!r) throw expectation_failure("failed parsing!");

    std::vector<PTR<ir::Expression> > ir;
    wrap::ir_prepend(ir);
    ir::Name lastval(NULL_VALUE);
    ir::convert(ast, ir, lastval);
    lastval = NULL_VALUE;
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
