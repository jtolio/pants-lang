#include "parser.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

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
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> list;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> mutation;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> definition;
    qi::rule<Iterator, std::vector<PTR<Expression> >(), ascii::space_type>
        appcommalist;
    qi::rule<Iterator, std::vector<PTR<Expression> >(), ascii::space_type>
        closedcallarglist;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> application;
    qi::rule<Iterator, std::vector<PTR<Term> >(), ascii::space_type> termlist;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> term;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> listexpansion;
    qi::rule<Iterator, PTR<Term>()> fullvalue;
    qi::rule<Iterator, PTR<ValueModifier>()> header;
    qi::rule<Iterator, PTR<ValueModifier>()> trailer;
    qi::rule<Iterator, PTR<ValueModifier>()> rightopencall;
    qi::rule<Iterator, PTR<ValueModifier>()> closedcall;
    qi::rule<Iterator, PTR<ValueModifier>()> index;
    qi::rule<Iterator, PTR<ValueModifier>()> field;
    qi::rule<Iterator, PTR<Value>()> value;
    qi::rule<Iterator, PTR<Value>()> subexpression;
    qi::rule<Iterator, PTR<Value>()> function;
    qi::rule<Iterator, PTR<Value>()> valvariable;
    qi::rule<Iterator, Variable()> variable;
    qi::rule<Iterator, PTR<Value>()> integer;
    qi::rule<Iterator, PTR<Value>()> bytestring;
    qi::rule<Iterator, PTR<Value>()> charstring;
    qi::rule<Iterator, PTR<Value>()> floating;
    qi::rule<Iterator, PTR<Value>()> map;
    qi::rule<Iterator, std::vector<MapDefinition>(), ascii::space_type>
        mapdefinitionlist;
    qi::rule<Iterator, MapDefinition(), ascii::space_type> mapdefinition;
    qi::rule<Iterator, std::string()> identifier;
    qi::rule<Iterator, std::string()> charstringvalue;
    qi::rule<Iterator, std::string()> bytestringvalue;
    qi::rule<Iterator, ArgList(), ascii::space_type> arglist;
    qi::rule<Iterator, std::vector<PTR<Argument> >(), ascii::space_type> argvec;
    qi::rule<Iterator, PTR<Argument>(), ascii::space_type> argument;
    qi::rule<Iterator, PTR<Argument>(), ascii::space_type> required_argument;
    qi::rule<Iterator, PTR<Argument>(), ascii::space_type> optional_argument;
    qi::rule<Iterator, PTR<Argument>(), ascii::space_type> arbitrary_argument;
    qi::rule<Iterator, PTR<Argument>(), ascii::space_type> keyword_argument;
    qi::rule<Iterator, PTR<Assignee>(), ascii::space_type> assignee;
    qi::rule<Iterator, PTR<Assignee>(), ascii::space_type> assignee_list;
    qi::rule<Iterator, std::vector<PTR<Assignee> >(), ascii::space_type>
        assignee_vect;
    qi::rule<Iterator, PTR<Assignee>(), ascii::space_type> single_assignee;
    
    grammar() : grammar::base_type(program) {
  
      program = -explist;
      program.name("program");
            
      explist = expression % ';' >> -qi::lit(';');
      explist.name("expression list");
      
      appcommalist = +(application >> ',') >> -application;
      appcommalist.name("list");
      list = appcommalist[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<List>(qi::_1))];
      list.name("list");
      
      termlist = +term;
      termlist.name("application");
      application = termlist[qi::_val = phx::construct<PTR<Expression> >(
          phx::new_<Application>(qi::_1))];
      application.name("application");
      
      expression = definition | mutation | list | application;
      expression.name("expression");
       
      term = listexpansion | fullvalue;
      term.name("term");
      
      listexpansion = (qi::lit("*(") >> explist >> ")")[
          qi::_val = phx::construct<PTR<Term> >(
          phx::new_<ListExpansion>(qi::_1))];
      listexpansion.name("list expansion");
      
      fullvalue = (*header >> value >> *trailer)[
          qi::_val = phx::construct<PTR<Term> >(phx::new_<FullValue>(qi::_1,
          qi::_2, qi::_3))];
      fullvalue.name("value with trailer");
      
      trailer = rightopencall | index | field | closedcall;
      trailer.name("value trailer");
            
      header = qi::char_("@")[
          qi::_val = phx::construct<PTR<ValueModifier> >(
          phx::new_<OpenCall>())];
      header.name("open call header");
      
      rightopencall = qi::char_("'?")[
          qi::_val = phx::construct<PTR<ValueModifier> >(
          phx::new_<OpenCall>())];
      rightopencall.name("open call trailer");
      
      closedcallarglist = *(application >> qi::lit(',')) >> -application;
      closedcallarglist.name("closed call argument list");
      closedcall = (qi::lit("(") >> qi::skip(ascii::space)[
          closedcallarglist >> qi::lit(")")])[qi::_val =
          phx::construct<PTR<ValueModifier> >(phx::new_<ClosedCall>(qi::_1))];
      closedcall.name("closed call trailer");
      
      index = (qi::lit("[") >> qi::skip(ascii::space)[explist >> qi::lit("]")])[
          qi::_val = phx::construct<PTR<ValueModifier> >(
          phx::new_<Index>(qi::_1))];
      index.name("index trailer");
      
      field = (qi::lit(".") >> variable)[
          qi::_val = phx::construct<PTR<ValueModifier> >(phx::new_<Field>(
          qi::_1))];
      field.name("field trailer");
      
      value = subexpression
            | function
            | bytestring
            | valvariable
            | integer
            | charstring
            | floating
            | map;
      value.name("value");
      
      subexpression = (qi::lit("(") >> qi::skip(ascii::space)[explist >> ")"])[
          qi::_val = phx::construct<PTR<Value> >(
          phx::new_<SubExpression>(qi::_1))];
      subexpression.name("subexpression");
      
      char const* exclude = " \n\r\t;,()[]{}|'\".?:=@";
      char const* digits = "0123456789";
      identifier = ((qi::char_ - qi::char_(exclude)) - qi::char_(digits)) >>
          *(qi::char_ - qi::char_(exclude));
      identifier.name("identifier");
      
      variable = identifier[phx::bind(&Variable::name, qi::_val) = qi::_1];
      variable.name("variable");
      
      valvariable = identifier[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<Variable>(qi::_1))];
      valvariable.name("variable value");
      
      integer = qi::long_long[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<Integer>(qi::_1))];
      integer.name("integer");
                
      floating = qi::double_[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<Float>(qi::_1))];
      floating.name("floating point number");
      
      charstringvalue = '"' >> +(qi::char_ - '"') >> '"';
      charstringvalue.name("character string");
      charstring = charstringvalue[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<CharString>(qi::_1))];
      charstring.name("character string");
      
      bytestringvalue = "b\"" >> +(qi::char_ - '"') >> '"';
      bytestringvalue.name("byte string");
      bytestring = bytestringvalue[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<ByteString>(qi::_1))];
      bytestring.name("byte string");
      
      map = (qi::lit("[") >> qi::skip(ascii::space)[mapdefinitionlist >>
          qi::lit("]")])[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<Map>(qi::_1))]; 
      map.name("map");
      
      mapdefinitionlist = *(mapdefinition >> qi::lit(',')) >> -mapdefinition;
      mapdefinitionlist.name("map definition list");
      
      mapdefinition = (application >> ':' >> application)[phx::bind(
          &MapDefinition::key, qi::_val) = qi::_1, phx::bind(
          &MapDefinition::value, qi::_val) = qi::_2];
      mapdefinition.name("map definition");
      
      function = (qi::lit("{") >> qi::skip(ascii::space)[-arglist] >>
          qi::skip(ascii::space)[program >> "}"])[qi::_val =
          phx::construct<PTR<Value> >(phx::new_<Function>(qi::_1, qi::_2))];
      function.name("function");
      arglist = (qi::lit("|") >> -(argvec >> ";") >> argvec >> "|")[
          phx::bind(&ArgList::left_args, qi::_val) = qi::_1,
          phx::bind(&ArgList::right_args, qi::_val) = qi::_2];
      arglist.name("argument list");
      
      argvec = *(argument >> ",") >> -argument;
      argvec.name("argument list");

      argument = keyword_argument
               | arbitrary_argument
               | optional_argument
               | required_argument;
      argument.name("argument");
      
      required_argument = variable[
          qi::_val = phx::construct<PTR<Argument> >(phx::new_<RequiredArgument>(
          qi::_1))];
      required_argument.name("required argument");
      
      optional_argument = (variable >> ":" >> fullvalue)[
          qi::_val = phx::construct<PTR<Argument> >(phx::new_<OptionalArgument>(
          qi::_1, qi::_2))];
      optional_argument.name("optional argument");
      
      arbitrary_argument = (qi::lit("*(") >> variable >> ")")[
          qi::_val = phx::construct<PTR<Argument> >(
          phx::new_<ArbitraryArgument>(qi::_1))];
      arbitrary_argument.name("arbitrary argument");
      
      keyword_argument = (qi::lit("**(") >> variable >> ")")[
          qi::_val = phx::construct<PTR<Argument> >(phx::new_<KeywordArgument>(
          qi::_1))];
      keyword_argument.name("keyword argument");
      
      mutation = (assignee >> ":=" >> expression)[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<Mutation>(
          qi::_1, qi::_2))];
      mutation.name("mutation");
      
      definition = (assignee >> "=" >> expression)[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<Definition>(
          qi::_1, qi::_2))];
      definition.name("definition");
      
      assignee = assignee_list | single_assignee;
      assignee.name("assignee");
      
      assignee_vect = +(single_assignee >> ',') >> -single_assignee;
      assignee_vect.name("assignee list");
      assignee_list = assignee_vect[qi::_val = phx::construct<PTR<Assignee> >(
          phx::new_<AssigneeList>(qi::_1))];
      assignee_list.name("assignee list");
      
      single_assignee = fullvalue[qi::_val = phx::construct<PTR<Assignee> >(
          phx::new_<SingleAssignee>(qi::_1))];
      single_assignee.name("single assignee");

      qi::on_error<qi::fail>(explist,
        std::cout << phx::val("Error! Expecting ") << qi::_4
                  << phx::val(" here: \"")
                  << phx::construct<std::string>(qi::_3, qi::_2)
                  << phx::val("\"") << std::endl
      );

    }
  };
}}

bool cirth::parser::parse(const std::string& src,
    std::vector<PTR<cirth::ast::Expression> >& ast) {
  grammar<std::string::const_iterator> g;
  std::string::const_iterator iter = src.begin();
  bool r = boost::spirit::qi::phrase_parse(iter, src.end(), g,
      boost::spirit::ascii::space, ast);
  return r && iter == src.end();
}
