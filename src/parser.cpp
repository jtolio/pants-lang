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
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> application;
    qi::rule<Iterator, std::vector<PTR<Term> >(), ascii::space_type> termlist;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> term;
    qi::rule<Iterator, PTR<Term>(), ascii::space_type> listexpansion;
    qi::rule<Iterator, PTR<Term>()> fullvalue;
    qi::rule<Iterator, PTR<Trailer>()> trailer;
    qi::rule<Iterator, PTR<Trailer>()> call;
    qi::rule<Iterator, PTR<Trailer>()> index;
    qi::rule<Iterator, PTR<Trailer>()> field;
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
    qi::rule<Iterator, HalfArgs(), ascii::space_type> leftargs;
    qi::rule<Iterator, HalfArgs(), ascii::space_type> rightargs;
    qi::rule<Iterator, HalfArgs(), ascii::space_type> leftargsnoopts;
    qi::rule<Iterator, HalfArgs(), ascii::space_type> rightargsnoopts;
    qi::rule<Iterator, OptionalArgs(), ascii::space_type> leftoptargs;
    qi::rule<Iterator, OptionalArgs(), ascii::space_type> rightoptargs;
    qi::rule<Iterator, VarArg()> vararg;
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
       
      term = fullvalue | listexpansion;
      term.name("term");
      
      listexpansion = (qi::lit("*(") >> explist >> ")")[
          qi::_val = phx::construct<PTR<Term> >(
          phx::new_<ListExpansion>(qi::_1))];
      listexpansion.name("list expansion");
      
      fullvalue = (value >> *trailer)[qi::_val = phx::construct<PTR<Term> >(
          phx::new_<FullValue>(qi::_1, qi::_2))];
      fullvalue.name("value with trailer");
      
      trailer = call | index | field;
      trailer.name("value trailer");
      
      call = qi::char_("'")[qi::_val = phx::construct<PTR<Trailer> >(
          phx::new_<Call>())];
      call.name("call trailer");
      
      index = (qi::lit("[") >> qi::skip(ascii::space)[explist >> qi::lit("]")])[
          qi::_val = phx::construct<PTR<Trailer> >(phx::new_<Index>(qi::_1))];
      index.name("index trailer");
      
      field = (qi::lit(".") >> variable)[
          qi::_val = phx::construct<PTR<Trailer> >(phx::new_<Field>(qi::_1))];
      field.name("field trailer");
      
      value = subexpression
            | function
            | valvariable
            | integer
            | bytestring
            | charstring
            | floating
            | map;
      value.name("value");
      
      subexpression = (qi::lit("(") >> qi::skip(ascii::space)[explist >> ")"])[
          qi::_val = phx::construct<PTR<Value> >(
          phx::new_<SubExpression>(qi::_1))];
      subexpression.name("subexpression");
      
      identifier = qi::alpha >> *qi::alnum;
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
      
      mapdefinitionlist = (*(mapdefinition >> qi::lit(',')) >> -(mapdefinition
          >> -qi::lit(',')));
      mapdefinitionlist.name("map definition list");
      
      mapdefinition = (application >> ':' >> application)[phx::bind(
          &MapDefinition::key, qi::_val) = qi::_1, phx::bind(
          &MapDefinition::value, qi::_val) = qi::_2];
      mapdefinition.name("map definition");
      
      function = (qi::lit("{") >> qi::skip(ascii::space)[-arglist] >>
          qi::skip(ascii::space)[program >> "}"])[qi::_val =
          phx::construct<PTR<Value> >(phx::new_<Function>(qi::_1, qi::_2))];
      function.name("function");
      arglist = (qi::lit("|") >> -((leftargs | leftargsnoopts) >> ';') >> 
          (rightargs | rightargsnoopts) >> qi::lit("|"))[
          phx::bind(&ArgList::leftargs, qi::_val) = qi::_1,
          phx::bind(&ArgList::rightargs, qi::_val) = qi::_2];
      arglist.name("argument list");
      vararg = (variable >> subexpression)[
          phx::bind(&VarArg::name, qi::_val) = qi::_1,
          phx::bind(&VarArg::subexpression, qi::_val) = qi::_2];
      vararg.name("variadic argument");
      leftoptargs = (-vararg >> *variable >> ',')[
          phx::bind(&OptionalArgs::var_arg, qi::_val) = qi::_1,
          phx::bind(&OptionalArgs::optional_args, qi::_val) = qi::_2];
      leftoptargs.name("left optional arguments");
      leftargs = (-leftoptargs >> *variable)[
          phx::bind(&HalfArgs::optional_args, qi::_val) = qi::_1,
          phx::bind(&HalfArgs::args, qi::_val) = qi::_2];
      leftargs.name("left arguments");
      leftargsnoopts = (vararg >> *variable)[
          phx::bind(&HalfArgs::var_arg, qi::_val) = qi::_1,
          phx::bind(&HalfArgs::args, qi::_val) = qi::_2];
      leftargsnoopts.name("left arguments with no optionals");
      rightoptargs = (qi::lit(',') >> *variable >> -vararg)[
          phx::bind(&OptionalArgs::optional_args, qi::_val) = qi::_1,
          phx::bind(&OptionalArgs::var_arg, qi::_val) = qi::_2];
      rightoptargs.name("right optional arguments");
      rightargs = (*variable >> -rightoptargs)[
          phx::bind(&HalfArgs::args, qi::_val) = qi::_1,
          phx::bind(&HalfArgs::optional_args, qi::_val) = qi::_2];
      rightargs.name("right arguments");
      rightargsnoopts = (*variable >> vararg)[
          phx::bind(&HalfArgs::var_arg, qi::_val) = qi::_2,
          phx::bind(&HalfArgs::args, qi::_val) = qi::_1];
      rightargsnoopts.name("right arguments with no optionals");
      
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
