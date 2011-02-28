#include "common.h"
#include "parser.h"
#include "wrap.h"
#include "pre_cps_ir.h"
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

  std::vector<PTR<pre_cps_ir::Expression> > ir;
  pre_cps_ir::convert(ast, ir);
  ast.clear();
  wrap::wrap(ir);

  return 0;
}
