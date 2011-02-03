#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "common.h"
#include "ast.h"

namespace cirth {
namespace parser {

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  namespace phx = boost::phoenix;
  using namespace cirth::ast;

  template <typename Iterator>
  struct grammar : qi::grammar<Iterator, std::vector<PTR<Expression> >(),
      ascii::space_type>{
    qi::rule<Iterator, std::vector<PTR<Expression> >(), ascii::space_type>
        program;
    qi::rule<Iterator, std::vector<PTR<Expression> >(), ascii::space_type>
        explist;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> expression;
//    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> reassignment;
//    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> definition;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> list;
    qi::rule<Iterator, std::vector<PTR<Expression> >(), ascii::space_type>
        appcommalist;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> application;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> term;
    qi::rule<Iterator, std::vector<PTR<Term> >(), ascii::space_type> termlist;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> value;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> listexpansion;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> subexpression;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> function;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> termvariable;
    qi::rule<Iterator, Variable(), ascii::space_type> variable;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> call;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> lookup;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> index;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> integer;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> bytestring;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> charstring;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> floating;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> map;
    qi::rule<Iterator, MapDefinition(), ascii::space_type> mapdefinition;
    qi::rule<Iterator, std::vector<MapDefinition>(), ascii::space_type>
        mapdefinitionlist;
    qi::rule<Iterator, Assignee(), ascii::space_type> assignee;
    qi::rule<Iterator, std::string(), ascii::space_type> identifier;
    qi::rule<Iterator, std::string(), ascii::space_type> charstringvalue;
    qi::rule<Iterator, std::string(), ascii::space_type> bytestringvalue;
    
    qi::rule<Iterator, ArgList(), ascii::space_type> arglist;
    qi::rule<Iterator, HalfArgs(), ascii::space_type> leftargs;
    qi::rule<Iterator, HalfArgs(), ascii::space_type> rightargs;
    qi::rule<Iterator, HalfArgs(), ascii::space_type> leftargsnoopts;
    qi::rule<Iterator, HalfArgs(), ascii::space_type> rightargsnoopts;
    qi::rule<Iterator, OptionalArgs(), ascii::space_type> leftoptargs;
    qi::rule<Iterator, OptionalArgs(), ascii::space_type> rightoptargs;
    qi::rule<Iterator, VarArg(), ascii::space_type> vararg;

    grammar() : grammar::base_type(program) {
      program = -explist;
      explist = expression % ';' >> -qi::lit(';');
      appcommalist = application >> ',' >> -(application % ',');
      termlist = +term;
      expression = list | application; // reassignment | definition
      list = appcommalist[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<List>(qi::_1))];
      application = termlist[qi::_val = phx::construct<PTR<Expression> >(
          phx::new_<Application>(qi::_1))];
      term = value | listexpansion;
      value = subexpression
              | function
              | termvariable
              | call
              | lookup
              | index
              | integer
              | bytestring
              | charstring
              | floating
              | map;
      listexpansion = (qi::lit("(") > explist > ")")[
          qi::_val = phx::construct<PTR<Term> >(
          phx::new_<ListExpansion>(qi::_1))];
      subexpression = (qi::lit("(") > explist > ")")[
          qi::_val = phx::construct<PTR<Term> >(
          phx::new_<SubExpression>(qi::_1))];
      identifier = qi::lexeme[qi::alpha >> *qi::alnum];
      variable = identifier[phx::bind(&Variable::name, qi::_val) = qi::_1];
      termvariable = identifier[qi::_val = phx::construct<PTR<Term> >(
          phx::new_<Variable>(qi::_1))];
      integer = qi::long_long[qi::_val = phx::construct<PTR<Term> >(
          phx::new_<Integer>(qi::_1))];
      floating = qi::double_[qi::_val = phx::construct<PTR<Term> >(
          phx::new_<Float>(qi::_1))];
      call = qi::lit("call")[
          qi::_val = phx::construct<PTR<Term> >()];
      lookup = qi::lit("lookup")[
          qi::_val = phx::construct<PTR<Term> >()];
      index = qi::lit("index")[
          qi::_val = phx::construct<PTR<Term> >()];
      charstringvalue = qi::lexeme['"' > +(qi::char_ - '"') > '"'];
      bytestringvalue = qi::lexeme["b\"" > +(qi::char_ - '"') > '"'];
      charstring = charstringvalue[qi::_val = phx::construct<PTR<Term> >(
          phx::new_<CharString>(qi::_1))];
      bytestring = bytestringvalue[qi::_val = phx::construct<PTR<Term> >(
          phx::new_<ByteString>(qi::_1))];
      map = (qi::lit("[") > mapdefinitionlist >> qi::lit("]"))[
          qi::_val = phx::construct<PTR<Term> >(phx::new_<Map>(qi::_1))]; 
      mapdefinitionlist = (*(mapdefinition >> qi::lit(',')) >> -(mapdefinition
          >> -qi::lit(',')))[qi::_val = qi::_1];
      mapdefinition = (application >> ':' >> application)[phx::bind(
          &MapDefinition::key, qi::_val) = qi::_1, phx::bind(
          &MapDefinition::value, qi::_val) = qi::_2];
      function = (qi::lit("{") > -arglist >> program >> qi::lit("}"))[
          qi::_val = phx::construct<PTR<Term> >(phx::new_<Function>(
          qi::_1, qi::_2))];
      arglist = (qi::lit("|") > -((leftargs | leftargsnoopts) >> ';') >> 
          (rightargs | rightargsnoopts) >> qi::lit("|"))[
          phx::bind(&ArgList::leftargs, qi::_val) = qi::_1,
          phx::bind(&ArgList::rightargs, qi::_val) = qi::_2];
      vararg = (variable >> subexpression)[
          phx::bind(&VarArg::name, qi::_val) = qi::_1,
          phx::bind(&VarArg::subexpression, qi::_val) = qi::_2];
      leftoptargs = (-vararg >> *variable >> ',')[
          phx::bind(&OptionalArgs::var_arg, qi::_val) = qi::_1,
          phx::bind(&OptionalArgs::optional_args, qi::_val) = qi::_2];
      leftargs = (-leftoptargs >> *variable)[
          phx::bind(&HalfArgs::optional_args, qi::_val) = qi::_1,
          phx::bind(&HalfArgs::args, qi::_val) = qi::_2];
      leftargsnoopts = (vararg >> *variable)[
          // TODO get vararg
          phx::bind(&HalfArgs::args, qi::_val) = qi::_2];
      rightoptargs = (qi::lit(',') >> *variable >> -vararg)[
          phx::bind(&OptionalArgs::optional_args, qi::_val) = qi::_1,
          phx::bind(&OptionalArgs::var_arg, qi::_val) = qi::_2];
      rightargs = (*variable >> -rightoptargs)[
          phx::bind(&HalfArgs::args, qi::_val) = qi::_1,
          phx::bind(&HalfArgs::optional_args, qi::_val) = qi::_2];
      rightargsnoopts = (*variable >> vararg)[
          // TODO get vararg
          phx::bind(&HalfArgs::args, qi::_val) = qi::_1];

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

  using namespace cirth;
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
