#include "common.h"
#include "parser.h"
#include "wrap.h"
#include "ir.h"
#include "cps.h"
#include "compile.h"
#include "optimize.h"
#include <iostream>
#include "assets.h"

using namespace pants;

int main(int argc, char** argv) {

  bool include_prelude = true;
  bool use_gc = true;

  for(int i = 1; i < argc; ++i) {
    if(argv[i] == std::string("--skip-prelude")) {
      include_prelude = false;
      continue;
    }
    if(argv[i] == std::string("--no-gc")) {
      use_gc = false;
      continue;
    }
    if(argv[i] == std::string("--help")) {
      std::cout << "usage: " << argv[0] << " [--skip-prelude]" << std::endl;
      std::cout << "  source comes in stdin, C comes out stdout" << std::endl;
      return 0;
    }
    std::cerr << "unknown argument! try --help" << std::endl;
    return 1;
  }

  std::string str;
  std::ostringstream os;
  if(include_prelude) os << assets::PRELUDE_P << "\n";

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
    lastval = NULL_VALUE;
    ast.clear();
    optimize::ir(ir);

    PTR<cps::Expression> cps;
    cps::transform(ir, lastval, cps);
    ir.clear();
    optimize::cps(cps);

    compile::compile(cps, std::cout, use_gc);
  } catch (const std::exception& e) {
    std::cerr << "failure: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
