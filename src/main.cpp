#include "common.h"
#include "parser.h"
#include "wrap.h"
#include "ir.h"
#include "cps.h"
#include <iostream>

int main(int argc, char** argv) {
  std::cout << "started" << std::endl;

  using namespace cirth;
  std::string str;
  std::ostringstream os;
  while(getline(std::cin, str)) {
    if(str.empty()) break;
    os << str;
  }
  str = os.str();

  std::vector<PTR<ast::Expression> > ast;
  bool r = parser::parse(str, ast);
  if(!r) {
    std::cerr << "failed parsing!" << std::endl;
    return -1;
  }

  std::vector<PTR<ir::Expression> > ir;
//  wrap::ir_prepend(ir);
  ir::Name lastval(NULL_VALUE);
  ir::convert(ast, ir, lastval);
  ast.clear();
  for(unsigned int i = 0; i < ir.size(); ++i)
    std::cout << ir[i]->format(0) << std::endl;
  PTR<cps::Expression> cps;
  cps::transform(ir, lastval, cps);
  ir.clear();
  std::cout << cps->format(0) << std::endl;

  return 0;
}
