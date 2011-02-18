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

  std::vector<PTR<ast::Expression> > p;
  bool r = parser::parse(str, p);
  if(!r) {
    std::cerr << "failed parsing!" << std::endl;
    return -1;
  }

  wrap::wrap(p);
  std::vector<PTR<pre_cps_ir::Expression> > ir;
  r = pre_cps_ir::convert(p, ir);
  if(!r) {
    std::cerr << "failed converting to IR" << std::endl;
    return -1;
  }

  return 0;
}
