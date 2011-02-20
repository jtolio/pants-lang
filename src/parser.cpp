#include "parser.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace cirth {
namespace parser {

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  namespace phx = boost::phoenix;
  using namespace cirth::ast;
  typedef std::string::const_iterator Iterator;

  struct grammar : qi::grammar<Iterator, std::vector<PTR<Expression> >()>{
    qi::rule<Iterator, std::vector<PTR<Expression> >()> program;
    qi::rule<Iterator, std::vector<PTR<Expression> >()> explist;
    qi::rule<Iterator, std::vector<PTR<Expression> >()> nl_explist;
    qi::rule<Iterator> nl_explistsep;
    qi::rule<Iterator, PTR<Expression>()> expression;
    qi::rule<Iterator, PTR<Expression>()> mutation;
    qi::rule<Iterator, PTR<Expression>()> definition;
    qi::rule<Iterator, PTR<Expression>()> application;
    qi::rule<Iterator, std::vector<PTR<Term> >()> termlist;
    qi::rule<Iterator, PTR<Expression>()> nl_expression;
    qi::rule<Iterator, PTR<Expression>()> nl_mutation;
    qi::rule<Iterator, PTR<Expression>()> nl_definition;
    qi::rule<Iterator, PTR<Expression>()> nl_application;
    qi::rule<Iterator, std::vector<PTR<Term> >()> nl_termlist;
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
    qi::rule<Iterator, std::vector<PTR<Expression> >()> array_elem_list;
    qi::rule<Iterator, std::vector<DictDefinition>()> dictdefinitionlist;
    qi::rule<Iterator, DictDefinition()> dictdefinition;
    qi::rule<Iterator, std::string()> identifier;
    qi::rule<Iterator, std::string()> charstringvalue;
    qi::rule<Iterator, std::string()> bytestringvalue;
    qi::rule<Iterator, InArgList()> inarglist;
    qi::rule<Iterator, std::vector<PTR<InArgument> >()> inargvec;
    qi::rule<Iterator, PTR<InArgument>()> in_argument;
    qi::rule<Iterator, PTR<InArgument>()> required_in_argument;
    qi::rule<Iterator, PTR<InArgument>()> optional_in_argument;
    qi::rule<Iterator, PTR<InArgument>()> arbitrary_in_argument;
    qi::rule<Iterator, PTR<InArgument>()> keyword_in_argument;
    qi::rule<Iterator, std::vector<PTR<OutArgument> >()> out_arguments;
    qi::rule<Iterator, PTR<OutArgument>()> out_argument;
    qi::rule<Iterator, PTR<OutArgument>()> required_out_argument;
    qi::rule<Iterator, PTR<OutArgument>()> optional_out_argument;
    qi::rule<Iterator, PTR<OutArgument>()> arbitrary_out_argument;
    qi::rule<Iterator, PTR<OutArgument>()> keyword_out_argument;
    qi::rule<Iterator, PTR<Assignee>()> assignee;
    qi::rule<Iterator> nl_skipper;
    qi::rule<Iterator> skipper;

    grammar() : grammar::base_type(program) {
#define S(exp) qi::skip(skipper.alias())[exp]
#define NLS(exp) qi::skip(nl_skipper.alias())[exp]

      nl_skipper = qi::char_(" \t\r");
      skipper = nl_skipper | '\n';

      program = -nl_explist;
      program.name("program");

      explist = S(*(expression >> +qi::lit(";")) >> expression >>
          *qi::lit(";"));
      explist.name("newline-indifferent expression list");

      nl_explistsep = qi::char_(";\n");
      nl_explistsep.name("newline-significant expression list separator");

      nl_explist = NLS(*(nl_expression >> +nl_explistsep) >> nl_expression >>
          *nl_explistsep);
      nl_explist.name("newline-significant expression list");

      expression = definition | mutation | application;
      expression.name("newline-indifferent expression");

      nl_expression = nl_definition | nl_mutation | nl_application;
      nl_expression.name("newline-significant expression");

      termlist = S(+term);
      termlist.name("newline-indifferent application");
      application = termlist[qi::_val = phx::construct<PTR<Expression> >(
          phx::new_<Application>(qi::_1))];
      application.name("newline-indifferent application");

      nl_termlist = NLS(+term);
      nl_termlist.name("newline-significant application");
      nl_application = nl_termlist[qi::_val = phx::construct<PTR<Expression> >(
          phx::new_<Application>(qi::_1))];
      nl_application.name("newline-significant application");

      mutation = (S(assignee) >> S(":=" >> expression))[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<Mutation>(
          qi::_1, qi::_2))];
      mutation.name("newline-indifferent mutation");

      nl_mutation = (NLS(assignee) >> NLS(":=" >> nl_expression))[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<Mutation>(
          qi::_1, qi::_2))];
      nl_mutation.name("newline-significant mutation");

      definition = (S(assignee) >> S("=" >> expression))[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<Definition>(
          qi::_1, qi::_2))];
      definition.name("newline-indifferent definition");

      nl_definition = (NLS(assignee) >> NLS("=" >> nl_expression))[
          qi::_val = phx::construct<PTR<Expression> >(phx::new_<Definition>(
          qi::_1, qi::_2))];
      nl_definition.name("newline-significant definition");

      assignee = term[qi::_val = phx::construct<PTR<Assignee> >(
          phx::new_<Assignee>(qi::_1))];
      assignee.name("assignee");

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

      optional_out_argument = (S(variable) >> S(":" >> application))[
          qi::_val = phx::construct<PTR<OutArgument> >(
          phx::new_<OptionalOutArgument>(qi::_1, qi::_2))];
      optional_out_argument.name("optional out argument");

      arbitrary_out_argument = (S(qi::lit("*(") >> explist >> ")"))[
          qi::_val = phx::construct<PTR<OutArgument> >(
          phx::new_<ArbitraryOutArgument>(qi::_1))];
      arbitrary_out_argument.name("arbitrary out argument");

      keyword_out_argument = (S(qi::lit("**(") >> explist >> ")"))[
          qi::_val = phx::construct<PTR<OutArgument> >(
          phx::new_<KeywordOutArgument>(qi::_1))];
      keyword_out_argument.name("keyword out argument");

      out_argument = keyword_out_argument
                   | arbitrary_out_argument
                   | optional_out_argument
                   | required_out_argument;
      out_argument.name("out argument");

      out_arguments = S(*(out_argument >> qi::lit(',')) >> -out_argument);
      out_arguments.name("out argument list");

      closedcall = (qi::lit("(") >> S(
          -(out_arguments >> ';') >>
          out_arguments >>
          -(qi::lit('|') >> out_arguments) >>
          ")"))[qi::_val =
          phx::construct<PTR<ValueModifier> >(phx::new_<ClosedCall>(qi::_1,
          qi::_2, qi::_3))];
      closedcall.name("closed call trailer");

      index = (qi::lit("[") >> S(explist >>
          qi::lit("]")))[qi::_val = phx::construct<PTR<ValueModifier> >(
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

      subexpression = (qi::lit("(") >> S(
          explist >> ")"))[qi::_val = phx::construct<PTR<Value> >(
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

      dictionary = (qi::lit("{") >> S(
          dictdefinitionlist >> qi::lit("}")))[
          qi::_val = phx::construct<PTR<Value> >(
          phx::new_<Dictionary>(qi::_1))];
      dictionary.name("dictionary");

      dictdefinitionlist = S(*(dictdefinition >> qi::lit(',')) >>
          -dictdefinition);
      dictdefinitionlist.name("dictionary definition list");

      dictdefinition = (S(application) >> S(':' >> application))[phx::bind(
          &DictDefinition::key, qi::_val) = qi::_1, phx::bind(
          &DictDefinition::value, qi::_val) = qi::_2];
      dictdefinition.name("dictionary definition");

      array = (qi::lit("[") >> S(array_elem_list >>
          qi::lit("]")))[qi::_val = phx::construct<PTR<Value> >(
          phx::new_<Array>(qi::_1))];
      array.name("array");

      array_elem_list = S(*(application >> qi::lit(',')) >> -application);
      array_elem_list.name("array element list");

      function = (qi::lit("{") >> S(-inarglist) >>
          S(nl_explist >> "}"))[qi::_val =
          phx::construct<PTR<Value> >(phx::new_<Function>(qi::_1, qi::_2))];
      function.name("function");
      inarglist = (S(qi::lit("|") >> -(inargvec >> ";")) >> S(inargvec >> "|"))[
          phx::bind(&InArgList::left_args, qi::_val) = qi::_1,
          phx::bind(&InArgList::right_args, qi::_val) = qi::_2];
      inarglist.name("in argument list");

      inargvec = S(*(in_argument >> ",") >> -in_argument);
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

      optional_in_argument = (S(variable) >> S(":" >> application))[
          qi::_val = phx::construct<PTR<InArgument> >(
          phx::new_<OptionalInArgument>(qi::_1, qi::_2))];
      optional_in_argument.name("optional in argument");

      arbitrary_in_argument = (S(qi::lit("*(") >> variable >> ")"))[
          qi::_val = phx::construct<PTR<InArgument> >(
          phx::new_<ArbitraryInArgument>(qi::_1))];
      arbitrary_in_argument.name("arbitrary in argument");

      keyword_in_argument = (S(qi::lit("**(") >> variable >> ")"))[
          qi::_val = phx::construct<PTR<InArgument> >(
          phx::new_<KeywordInArgument>(qi::_1))];
      keyword_in_argument.name("keyword in argument");

#undef S

      qi::on_error<qi::fail>(program,
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
  grammar g;
  std::string::const_iterator iter = src.begin();
  bool r = boost::spirit::qi::phrase_parse(iter, src.end(), g, ascii::space,
      ast);
  return r && iter == src.end();
}
