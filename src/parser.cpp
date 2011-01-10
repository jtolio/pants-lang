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
  struct grammar : qi::grammar<Iterator, std::vector<PTR<Expression> >(),
      ascii::space_type>{
    qi::rule<Iterator, std::vector<PTR<Expression> >(), ascii::space_type>
        explist;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> expression;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> assignment;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> reassignment;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> definition;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> list;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> application;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> term;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> value;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> listexpansion;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> function;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> variable;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> call;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> lookup;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> index;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> integer;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> bytestring;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> charstring;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> floating;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> map;
    qi::rule<Iterator, Assignee(), ascii::space_type> assignee;

    grammar() : grammar::base_type(explist) {

      explist = expression % ';' >> -qi::lit(';');

      expression = qi::lit("x")[qi::_val = phx::construct<PTR<Expression> >()];

      qi::on_error<qi::fail>(explist,
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
  std::vector<PTR<ast::Expression> > p;
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
