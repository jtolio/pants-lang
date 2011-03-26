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

  std::vector<PTR<ir::Assignment> > ir;
  PTR<ir::Value> lastval;
  ir::convert(ast, ir, lastval);
  ast.clear();
//  std::vector<PTR<ir::Expression> > cps;
//  cps::transform(ir, cps);
//  ir.clear();
//  wrap::wrap(cps);

//  for(unsigned int i = 0; i < cps.size(); ++i) {
//    std::cout << cps[i]->format() << std::endl;
//  }

  return 0;
}
