#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "common.h"
#include "ast.h"

namespace jtlang {
namespace parser {

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  namespace phx = boost::phoenix;
  using namespace jtlang::ast;
  
  template <typename Iterator>
  struct grammar : qi::grammar<Iterator, Program(), ascii::space_type>{
    qi::rule<Iterator, Program(), ascii::space_type> program;
    qi::rule<Iterator, std::vector<PTR<Expression> >(), ascii::space_type>
        explist;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> expression;

    grammar() : grammar::base_type(program) {

      qi::on_error<qi::fail>(program,
        std::cout << phx::val("Error! Expecting ") << qi::_4
                  << phx::val(" here: \"")
                  << phx::construct<std::string>(qi::_3, qi::_2)
                  << phx::val("\"") << std::endl
      );
    }
  };
}}

int main(int argc, char** argv) {
  std::cout << "started" << std::endl;

  using namespace jtlang;
  std::string str;
  std::ostringstream os;
  while(getline(std::cin, str)) {
    if(str.empty()) break;
    os << str;
  }
  str = os.str();
  
  parser::grammar<std::string::const_iterator> g;
  ast::Program p;
  std::string::const_iterator iter = str.begin();
  std::string::const_iterator end = str.end();
  bool r = boost::spirit::qi::phrase_parse(iter, end, g,
      boost::spirit::ascii::space, p);
  if(r && iter == str.end()) {
    std::cout << "yay" << std::endl;
  } else {
    std::cout << "boo" << std::endl;
  }
  // do stuff
  return 0;
}
