#include "common.h"
#include "parser.h"
#include "wrap.h"
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
  if(r) {
    wrap::wrap(p);
    for(unsigned int i = 0; i < p.size(); ++i) {
      std::cout << p[i]->format() << std::endl;
    }
  } else {
    std::cout << "Failed parsing!" << std::endl;
  }

  return 0;
}
