#include "common.h"
#include "parser.h"
#include "ir.h"
#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

class ParserTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(ParserTest);
  CPPUNIT_TEST(testSimpleParse);
  CPPUNIT_TEST(testArrayVsIndex);
  CPPUNIT_TEST(testCallVsField);
  CPPUNIT_TEST(testSingleLists);
  CPPUNIT_TEST(testParses);
  CPPUNIT_TEST(testListExpansion);
  CPPUNIT_TEST(testByteString);
  CPPUNIT_TEST(testClosedCall);
  CPPUNIT_TEST(testFunctions);
  CPPUNIT_TEST(testNewlines);
  CPPUNIT_TEST(testFloatingPoint);
  CPPUNIT_TEST(testEquality);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}
  void testSimpleParse() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("hey' there", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(hey, user_provided)), Trailers(OpenCall())), Term(Value(Variable(there, user_provided)), Trailers()))");
  }

  void testArrayVsIndex() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing [value]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(thing, user_provided)), Trailers()), Term(Value(Array(Application(Term(Value(Variable(value, user_provided)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing[key]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(thing, user_provided)), Trailers(Index(Application(Term(Value(Variable(key, user_provided)), Trailers()))))))");
  }

  void testCallVsField() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing1' notafield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(thing1, user_provided)), Trailers(OpenCall())), Term(Value(Variable(notafield, user_provided)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing1.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(thing1, user_provided)), Trailers(Field(Variable(afield, user_provided)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' function'.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(function, user_provided)), Trailers(OpenCall(), Field(Variable(afield, user_provided)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' function'.afuncfield'", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(function, user_provided)), Trailers(OpenCall(), Field(Variable(afuncfield, user_provided)), OpenCall())))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call. thing1. notafield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(thing1, user_provided)), Trailers(OpenCall())), Term(Value(Variable(notafield, user_provided)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call. thing1.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(thing1, user_provided)), Trailers(Field(Variable(afield, user_provided)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call. function..afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(function, user_provided)), Trailers(OpenCall(), Field(Variable(afield, user_provided)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call. function..afuncfield.", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided)), Trailers(OpenCall())), Term(Value(Variable(function, user_provided)), Trailers(OpenCall(), Field(Variable(afuncfield, user_provided)), OpenCall())))");
  }

  void testClosedCall() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(arg, user_provided)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1, arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided)), Trailers()))), RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f (arg)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers()), Term(Value(SubExpression(Application(Term(Value(Variable(arg, user_provided)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f'(arg)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(OpenCall(), ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(arg, user_provided)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f' (arg)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(OpenCall())), Term(Value(SubExpression(Application(Term(Value(Variable(arg, user_provided)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1;arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided)), Trailers())))), Right(RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1,arg2;arg3)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided)), Trailers()))), RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided)), Trailers())))), Right(RequiredOutArgument(Application(Term(Value(Variable(arg3, user_provided)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1,arg2,;arg3)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided)), Trailers()))), RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided)), Trailers())))), Right(RequiredOutArgument(Application(Term(Value(Variable(arg3, user_provided)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1;arg2|arg3:3)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided)), Trailers())))), Right(RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided)), Trailers())))), Scoped(OptionalOutArgument(Variable(arg3, user_provided), Application(Term(Value(Integer(3)), Trailers()))))))))");
  }

  void testSingleLists() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("[z,]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Array(Application(Term(Value(Variable(z, user_provided)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{test1:test2,}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Dictionary(DictDefinition(Application(Term(Value(Variable(test1, user_provided)), Trailers())), Application(Term(Value(Variable(test2, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1,)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,;| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided)))), Right(Required(), Optional()), Expressions(Application(Term(Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|b,| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(b, user_provided))), Optional()), Expressions(Application(Term(Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
  }

  void testFunctions() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("{}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Dictionary()), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(!cirth::parser::parse("{|| thing:thing}", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{thing1:thing2}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Dictionary(DictDefinition(Application(Term(Value(Variable(thing1, user_provided)), Trailers())), Application(Term(Value(Variable(thing2, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(), Optional()), Expressions(Application(Term(Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{{thing1:thing2}}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(), Optional()), Expressions(Application(Term(Value(Dictionary(DictDefinition(Application(Term(Value(Variable(thing1, user_provided)), Trailers())), Application(Term(Value(Variable(thing2, user_provided)), Trailers()))))), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a| print(\"hi\"); null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided))), Optional()), Expressions(Application(Term(Value(Variable(print, user_provided)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(CharString(hi)), Trailers())))), Scoped())))), Application(Term(Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided))), Optional()), Expressions(Application(Term(Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided))), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided))), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided))), Optional(OptionalInArgument(Variable(c, user_provided), Application(Term(Value(Integer(3)), Trailers()))))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3,d:4| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided))), Optional(OptionalInArgument(Variable(c, user_provided), Application(Term(Value(Integer(3)), Trailers()))), OptionalInArgument(Variable(d, user_provided), Application(Term(Value(Integer(4)), Trailers()))))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3,d:4,*(opt)| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided))), Optional(OptionalInArgument(Variable(c, user_provided), Application(Term(Value(Integer(3)), Trailers()))), OptionalInArgument(Variable(d, user_provided), Application(Term(Value(Integer(4)), Trailers())))), Arbitrary(Variable(opt, user_provided))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(!cirth::parser::parse("{|a,b,c:3,d:4,q(opt)| 0}", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|;| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a;| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided)))), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b;| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided)))), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,;| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided)))), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(var),a,b,;| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided))), Arbitrary(Variable(var, user_provided))), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(var),a,b,d;e,f,g:5,h:7,*(j)| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided)), RequiredInArgument(Variable(b, user_provided)), RequiredInArgument(Variable(d, user_provided))), Arbitrary(Variable(var, user_provided))), Right(Required(RequiredInArgument(Variable(e, user_provided)), RequiredInArgument(Variable(f, user_provided))), Optional(OptionalInArgument(Variable(g, user_provided), Application(Term(Value(Integer(5)), Trailers()))), OptionalInArgument(Variable(h, user_provided), Application(Term(Value(Integer(7)), Trailers())))), Arbitrary(Variable(j, user_provided))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(a);*(b)| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(), Arbitrary(Variable(a, user_provided))), Right(Required(), Optional(), Arbitrary(Variable(b, user_provided))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|;**(b)| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(), Optional(), Keyword(Variable(b, user_provided))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
  }

  void testListExpansion() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f(thing, *(thing))", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(thing, user_provided)), Trailers()))), ArbitraryOutArgument(Application(Term(Value(Variable(thing, user_provided)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(*,(thing),thing)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(*, user_provided)), Trailers()))), RequiredOutArgument(Application(Term(Value(SubExpression(Application(Term(Value(Variable(thing, user_provided)), Trailers())))), Trailers()))), RequiredOutArgument(Application(Term(Value(Variable(thing, user_provided)), Trailers())))), Scoped()))))");
  }

  void testByteString() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f' b\"thing\"", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(OpenCall())), Term(Value(ByteString(thing)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f' b \"thing\"", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers(OpenCall())), Term(Value(Variable(b, user_provided)), Trailers()), Term(Value(CharString(thing)), Trailers()))");
  }

  void testParses() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("x = 1", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(a b).thing", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(a b).thing = 3", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("z.thing = 3; x[4] = 5", exps));
    exps.clear();
  }

  void testNewlines() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f; f", exps));
    CPPUNIT_ASSERT(exps.size() == 2);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers()))");
    CPPUNIT_ASSERT(exps[1]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f\nf", exps));
    CPPUNIT_ASSERT(exps.size() == 2);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers()))");
    CPPUNIT_ASSERT(exps[1]->format() == "Application(Term(Value(Variable(f, user_provided)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("hey there; how are you; ", exps));
    CPPUNIT_ASSERT(exps.size() == 2);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(hey, user_provided)), Trailers()), Term(Value(Variable(there, user_provided)), Trailers()))");
    CPPUNIT_ASSERT(exps[1]->format() == "Application(Term(Value(Variable(how, user_provided)), Trailers()), Term(Value(Variable(are, user_provided)), Trailers()), Term(Value(Variable(you, user_provided)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("hey there\nhow are you\n", exps));
    CPPUNIT_ASSERT(exps.size() == 2);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(hey, user_provided)), Trailers()), Term(Value(Variable(there, user_provided)), Trailers()))");
    CPPUNIT_ASSERT(exps[1]->format() == "Application(Term(Value(Variable(how, user_provided)), Trailers()), Term(Value(Variable(are, user_provided)), Trailers()), Term(Value(Variable(you, user_provided)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(f\nf)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(SubExpression(Application(Term(Value(Variable(f, user_provided)), Trailers()), Term(Value(Variable(f, user_provided)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(f;f)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(SubExpression(Application(Term(Value(Variable(f, user_provided)), Trailers())), Application(Term(Value(Variable(f, user_provided)), Trailers())))), Trailers()))");
  }
  
  void testFloatingPoint() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("x := 3\n", exps));  
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0]->format() == "Definition(Variable(x, user_provided), Application(Term(Value(Integer(3)), Trailers())))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("x := 3.0\n", exps));  
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0]->format() == "Definition(Variable(x, user_provided), Application(Term(Value(Float(3.0)), Trailers())))");
  }
  
  void testEquality() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("x <. 3\n", exps));    
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(x, user_provided)), Trailers()), Term(Value(Variable(<, user_provided)), Trailers(OpenCall())), Term(Value(Integer(3)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("x ==. 3\n", exps));    
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(x, user_provided)), Trailers()), Term(Value(Variable(==, user_provided)), Trailers(OpenCall())), Term(Value(Integer(3)), Trailers()))");
  }

};

class IRTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(IRTest);
  CPPUNIT_TEST(testSimple);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}
  std::string ir_translate(const std::string& src) {
    std::vector<PTR<cirth::ast::Expression> > ast;
    CPPUNIT_ASSERT(cirth::parser::parse(src, ast));
    std::vector<PTR<cirth::ir::Expression> > ir;
    cirth::ir::Name lastval(NULL_VALUE);
    cirth::ir::convert(ast, ir, lastval);
    std::ostringstream out;
    for(unsigned int i = 0; i < ir.size(); ++i)
      out << ir[i]->format(0) << std::endl;
    out << lastval.format();
    return out.str();
  }

  void testSimple() {
    CPPUNIT_ASSERT(ir_translate("x := { 3 }\nx.\n") ==
        "Definition(u_x, Variable(c_null))\n"
        "Definition(c_ir_2, Function(Left(), Right(), "
            "Expressions(Definition(c_ir_1, Integer(3))), LastVal(c_ir_1)))\n"
        "VariableMutation(u_x, c_ir_2)\n"
        "ReturnValue(c_ir_3, Call(u_x, Left(), Right(), Scoped()))\n"
        "c_ir_3");
    CPPUNIT_ASSERT(ir_translate("x := { 3 }\nx()\n") ==
        "Definition(u_x, Variable(c_null))\n"
        "Definition(c_ir_2, Function(Left(), Right(), "
            "Expressions(Definition(c_ir_1, Integer(3))), LastVal(c_ir_1)))\n"
        "VariableMutation(u_x, c_ir_2)\n"
        "ReturnValue(c_ir_3, Call(u_x, Left(), Right(), Scoped()))\n"
        "c_ir_3");
    CPPUNIT_ASSERT(ir_translate("x := { 3 }\nx.") ==
        "Definition(u_x, Variable(c_null))\n"
        "Definition(c_ir_2, Function(Left(), Right(), "
            "Expressions(Definition(c_ir_1, Integer(3))), LastVal(c_ir_1)))\n"
        "VariableMutation(u_x, c_ir_2)\n"
        "ReturnValue(c_ir_3, Call(u_x, Left(), Right(), Scoped()))\n"
        "c_ir_3");
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ParserTest);
CPPUNIT_TEST_SUITE_REGISTRATION(IRTest);

int main(int argc, char** argv) {
  CPPUNIT_NS::TextUi::TestRunner runner;
  runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
  runner.setOutputter(new CPPUNIT_NS::CompilerOutputter(&runner.result(),
      CPPUNIT_NS::stdCOut()));
  return runner.run() ? 0 : 1;
}
