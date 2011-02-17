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
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> mutation;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> definition;
    qi::rule<Iterator, PTR<Expression>(), ascii::space_type> application;
    qi::rule<Iterator, std::vector<PTR<Term> >(), ascii::space_type> termlist;
    qi::rule<Iterator, PTR<Term>()> term;
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
    qi::rule<Iterator, PTR<Value>()> dictionary;
    qi::rule<Iterator, PTR<Value>()> array;
    qi::rule<Iterator, std::vector<PTR<Expression> >(), ascii::space_type>
        array_elem_list;
    qi::rule<Iterator, std::vector<DictDefinition>(), ascii::space_type>
        dictdefinitionlist;
    qi::rule<Iterator, DictDefinition(), ascii::space_type> dictdefinition;
    qi::rule<Iterator, std::string()> identifier;
    qi::rule<Iterator, std::string()> charstringvalue;
    qi::rule<Iterator, std::string()> bytestringvalue;
    qi::rule<Iterator, InArgList(), ascii::space_type> inarglist;
    qi::rule<Iterator, std::vector<PTR<InArgument> >(), ascii::space_type>
        inargvec;
    qi::rule<Iterator, PTR<InArgument>(), ascii::space_type> in_argument;
    qi::rule<Iterator, PTR<InArgument>(), ascii::space_type>
        required_in_argument;
    qi::rule<Iterator, PTR<InArgument>(), ascii::space_type>
        optional_in_argument;
    qi::rule<Iterator, PTR<InArgument>(), ascii::space_type>
        arbitrary_in_argument;
    qi::rule<Iterator, PTR<InArgument>(), ascii::space_type>
        keyword_in_argument;
    qi::rule<Iterator, std::vector<PTR<OutArgument> >(), ascii::space_type>
        out_arguments;
    qi::rule<Iterator, PTR<OutArgument>(), ascii::space_type> out_argument;
    qi::rule<Iterator, PTR<OutArgument>(), ascii::space_type>
        required_out_argument;
    qi::rule<Iterator, PTR<OutArgument>(), ascii::space_type>
        optional_out_argument;
    qi::rule<Iterator, PTR<OutArgument>(), ascii::space_type>
        arbitrary_out_argument;
    qi::rule<Iterator, PTR<OutArgument>(), ascii::space_type>
        keyword_out_argument;
    qi::rule<Iterator, PTR<Assignee>(), ascii::space_type> assignee;

    grammar() : grammar::base_type(program) {

      program = -explist;
      program.name("program");

      explist = expression % ';' >> -qi::lit(';');
      explist.name("expression list");

      termlist = +term;
      termlist.name("application");
      application = termlist[qi::_val = phx::construct<PTR<Expression> >(
          phx::new_<Application>(qi::_1))];
      application.name("application");

      expression = definition | mutation | application;
      expression.name("expression");

      term = (*header >> value >> *trailer)[
          qi::_val = phx::construct<PTR<Term> >(phx::new_<Term>(qi::_1,
          qi::_2, qi::_3))];
      term.name("term");

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

      required_out_argument = application[
          qi::_val = phx::construct<PTR<OutArgument> >(
          phx::new_<RequiredOutArgument>(qi::_1))];
      required_out_argument.name("required out argument");

      optional_out_argument = (variable >> ":" >> application)[
          qi::_val = phx::construct<PTR<OutArgument> >(
          phx::new_<OptionalOutArgument>(qi::_1, qi::_2))];
      optional_out_argument.name("optional out argument");

      arbitrary_out_argument = (qi::lit("*(") >> explist >> ")")[
          qi::_val = phx::construct<PTR<OutArgument> >(
          phx::new_<ArbitraryOutArgument>(qi::_1))];
      arbitrary_out_argument.name("arbitrary out argument");

      keyword_out_argument = (qi::lit("**(") >> explist >> ")")[
          qi::_val = phx::construct<PTR<OutArgument> >(
          phx::new_<KeywordOutArgument>(qi::_1))];
      keyword_out_argument.name("keyword out argument");

      out_argument = keyword_out_argument
                   | arbitrary_out_argument
                   | optional_out_argument
                   | required_out_argument;
      out_argument.name("out argument");

      out_arguments = *(out_argument >> qi::lit(',')) >> -out_argument;
      out_arguments.name("out argument list");

      closedcall = (qi::lit("(") >> qi::skip(ascii::space)[
          -(out_arguments >> ';') >> out_arguments >>
          -(qi::lit('|') >> out_arguments) >> ")"])[qi::_val =
          phx::construct<PTR<ValueModifier> >(phx::new_<ClosedCall>(qi::_1,
          qi::_2, qi::_3))];
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
            | dictionary
            | array;
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

      dictionary = (qi::lit("{") >> qi::skip(ascii::space)[dictdefinitionlist >>
          qi::lit("}")])[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<Dictionary>(qi::_1))];
      dictionary.name("dictionary");

      dictdefinitionlist = *(dictdefinition >> qi::lit(',')) >> -dictdefinition;
      dictdefinitionlist.name("dictionary definition list");

      dictdefinition = (application >> ':' >> application)[phx::bind(
          &DictDefinition::key, qi::_val) = qi::_1, phx::bind(
          &DictDefinition::value, qi::_val) = qi::_2];
      dictdefinition.name("dictionary definition");

      array = (qi::lit("[") >> qi::skip(ascii::space)[array_elem_list >>
          qi::lit("]")])[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<Array>(qi::_1))];
      array.name("array");

      array_elem_list = *(application >> qi::lit(',')) >> -application;
      array_elem_list.name("array element list");

      function = (qi::lit("{") >> qi::skip(ascii::space)[-inarglist] >>
          qi::skip(ascii::space)[explist >> "}"])[qi::_val =
          phx::construct<PTR<Value> >(phx::new_<Function>(qi::_1, qi::_2))];
      function.name("function");
      inarglist = (qi::lit("|") >> -(inargvec >> ";") >> inargvec >> "|")[
          phx::bind(&InArgList::left_args, qi::_val) = qi::_1,
          phx::bind(&InArgList::right_args, qi::_val) = qi::_2];
      inarglist.name("in argument list");

      inargvec = *(in_argument >> ",") >> -in_argument;
      inargvec.name("in argument list");

      in_argument = keyword_in_argument
               | arbitrary_in_argument
               | optional_in_argument
               | required_in_argument;
      in_argument.name("in argument");

      required_in_argument = variable[
          qi::_val = phx::construct<PTR<InArgument> >(
          phx::new_<RequiredInArgument>(qi::_1))];
      required_in_argument.name("required in argument");

      optional_in_argument = (variable >> ":" >> application)[
          qi::_val = phx::construct<PTR<InArgument> >(
          phx::new_<OptionalInArgument>(qi::_1, qi::_2))];
      optional_in_argument.name("optional in argument");

      arbitrary_in_argument = (qi::lit("*(") >> variable >> ")")[
          qi::_val = phx::construct<PTR<InArgument> >(
          phx::new_<ArbitraryInArgument>(qi::_1))];
      arbitrary_in_argument.name("arbitrary in argument");

      keyword_in_argument = (qi::lit("**(") >> variable >> ")")[
          qi::_val = phx::construct<PTR<InArgument> >(
          phx::new_<KeywordInArgument>(qi::_1))];
      keyword_in_argument.name("keyword in argument");

      mutation = (assignee >> ":=" >> expression)[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<Mutation>(
          qi::_1, qi::_2))];
      mutation.name("mutation");

      definition = (assignee >> "=" >> expression)[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<Definition>(
          qi::_1, qi::_2))];
      definition.name("definition");

      assignee = term[qi::_val = phx::construct<PTR<Assignee> >(
          phx::new_<Assignee>(qi::_1))];
      assignee.name("assignee");

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
