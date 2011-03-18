#include "common.h"
#include "parser.h"
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
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}
  void testSimpleParse() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("hey' there", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(hey, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(there, user_provided, unscoped)), Trailers()))");
  }

  void testArrayVsIndex() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing [value]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(thing, user_provided, unscoped)), Trailers()), Term(Value(Array(Application(Term(Value(Variable(value, user_provided, unscoped)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing[key]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(thing, user_provided, unscoped)), Trailers(Index(Application(Term(Value(Variable(key, user_provided, unscoped)), Trailers()))))))");
  }

  void testCallVsField() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing1' notafield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(thing1, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(notafield, user_provided, unscoped)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing1.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(thing1, user_provided, unscoped)), Trailers(Field(Variable(afield, user_provided, unscoped)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' function'.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(function, user_provided, unscoped)), Trailers(OpenCall(), Field(Variable(afield, user_provided, unscoped)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' function'.afuncfield'", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(function, user_provided, unscoped)), Trailers(OpenCall(), Field(Variable(afuncfield, user_provided, unscoped)), OpenCall())))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call. thing1. notafield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(thing1, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(notafield, user_provided, unscoped)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call. thing1.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(thing1, user_provided, unscoped)), Trailers(Field(Variable(afield, user_provided, unscoped)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call. function..afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(function, user_provided, unscoped)), Trailers(OpenCall(), Field(Variable(afield, user_provided, unscoped)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call. function..afuncfield.", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(call, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(function, user_provided, unscoped)), Trailers(OpenCall(), Field(Variable(afuncfield, user_provided, unscoped)), OpenCall())))");
  }

  void testClosedCall() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(arg, user_provided, unscoped)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1, arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided, unscoped)), Trailers()))), RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided, unscoped)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f (arg)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers()), Term(Value(SubExpression(Application(Term(Value(Variable(arg, user_provided, unscoped)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f'(arg)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(OpenCall(), ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(arg, user_provided, unscoped)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f' (arg)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(SubExpression(Application(Term(Value(Variable(arg, user_provided, unscoped)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1;arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided, unscoped)), Trailers())))), Right(RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided, unscoped)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1,arg2;arg3)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided, unscoped)), Trailers()))), RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided, unscoped)), Trailers())))), Right(RequiredOutArgument(Application(Term(Value(Variable(arg3, user_provided, unscoped)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1,arg2,;arg3)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided, unscoped)), Trailers()))), RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided, unscoped)), Trailers())))), Right(RequiredOutArgument(Application(Term(Value(Variable(arg3, user_provided, unscoped)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1;arg2|arg3:3)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided, unscoped)), Trailers())))), Right(RequiredOutArgument(Application(Term(Value(Variable(arg2, user_provided, unscoped)), Trailers())))), Scoped(OptionalOutArgument(Variable(arg3, user_provided, unscoped), Application(Term(Value(Integer(3)), Trailers()))))))))");
  }

  void testSingleLists() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("[z,]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Array(Application(Term(Value(Variable(z, user_provided, unscoped)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{test1:test2,}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Dictionary(DictDefinition(Application(Term(Value(Variable(test1, user_provided, unscoped)), Trailers())), Application(Term(Value(Variable(test2, user_provided, unscoped)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1,)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(arg1, user_provided, unscoped)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,;| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided, unscoped)))), Right(Required(), Optional()), Expressions(Application(Term(Value(Variable(null, user_provided, unscoped)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|b,| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(b, user_provided, unscoped))), Optional()), Expressions(Application(Term(Value(Variable(null, user_provided, unscoped)), Trailers()))))), Trailers()))");
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
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Dictionary(DictDefinition(Application(Term(Value(Variable(thing1, user_provided, unscoped)), Trailers())), Application(Term(Value(Variable(thing2, user_provided, unscoped)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(), Optional()), Expressions(Application(Term(Value(Variable(null, user_provided, unscoped)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{{thing1:thing2}}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(), Optional()), Expressions(Application(Term(Value(Dictionary(DictDefinition(Application(Term(Value(Variable(thing1, user_provided, unscoped)), Trailers())), Application(Term(Value(Variable(thing2, user_provided, unscoped)), Trailers()))))), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a| print(\"hi\"); null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided, unscoped))), Optional()), Expressions(Application(Term(Value(Variable(print, user_provided, unscoped)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(CharString(hi)), Trailers())))), Scoped())))), Application(Term(Value(Variable(null, user_provided, unscoped)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided, unscoped))), Optional()), Expressions(Application(Term(Value(Variable(null, user_provided, unscoped)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped))), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped))), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped))), Optional(OptionalInArgument(Variable(c, user_provided, unscoped), Application(Term(Value(Integer(3)), Trailers()))))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3,d:4| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped))), Optional(OptionalInArgument(Variable(c, user_provided, unscoped), Application(Term(Value(Integer(3)), Trailers()))), OptionalInArgument(Variable(d, user_provided, unscoped), Application(Term(Value(Integer(4)), Trailers()))))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3,d:4,*(opt)| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped))), Optional(OptionalInArgument(Variable(c, user_provided, unscoped), Application(Term(Value(Integer(3)), Trailers()))), OptionalInArgument(Variable(d, user_provided, unscoped), Application(Term(Value(Integer(4)), Trailers())))), Arbitrary(Variable(opt, user_provided, unscoped))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
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
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided, unscoped)))), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b;| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped)))), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,;| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped)))), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(var),a,b,;| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped))), Arbitrary(Variable(var, user_provided, unscoped))), Right(Required(), Optional()), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(var),a,b,d;e,f,g:5,h:7,*(j)| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(RequiredInArgument(Variable(a, user_provided, unscoped)), RequiredInArgument(Variable(b, user_provided, unscoped)), RequiredInArgument(Variable(d, user_provided, unscoped))), Arbitrary(Variable(var, user_provided, unscoped))), Right(Required(RequiredInArgument(Variable(e, user_provided, unscoped)), RequiredInArgument(Variable(f, user_provided, unscoped))), Optional(OptionalInArgument(Variable(g, user_provided, unscoped), Application(Term(Value(Integer(5)), Trailers()))), OptionalInArgument(Variable(h, user_provided, unscoped), Application(Term(Value(Integer(7)), Trailers())))), Arbitrary(Variable(j, user_provided, unscoped))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(a);*(b)| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required(), Arbitrary(Variable(a, user_provided, unscoped))), Right(Required(), Optional(), Arbitrary(Variable(b, user_provided, unscoped))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|;**(b)| 0}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Function(Left(Required()), Right(Required(), Optional(), Keyword(Variable(b, user_provided, unscoped))), Expressions(Application(Term(Value(Integer(0)), Trailers()))))), Trailers()))");
  }

  void testListExpansion() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f(thing, *(thing))", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(thing, user_provided, unscoped)), Trailers()))), ArbitraryOutArgument(Application(Term(Value(Variable(thing, user_provided, unscoped)), Trailers())))), Scoped()))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(*,(thing),thing)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(ClosedCall(Left(), Right(RequiredOutArgument(Application(Term(Value(Variable(*, user_provided, unscoped)), Trailers()))), RequiredOutArgument(Application(Term(Value(SubExpression(Application(Term(Value(Variable(thing, user_provided, unscoped)), Trailers())))), Trailers()))), RequiredOutArgument(Application(Term(Value(Variable(thing, user_provided, unscoped)), Trailers())))), Scoped()))))");
  }

  void testByteString() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f' b\"thing\"", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(ByteString(thing)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f' b \"thing\"", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers(OpenCall())), Term(Value(Variable(b, user_provided, unscoped)), Trailers()), Term(Value(CharString(thing)), Trailers()))");
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
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers()))");
    CPPUNIT_ASSERT(exps[1]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f\nf", exps));
    CPPUNIT_ASSERT(exps.size() == 2);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers()))");
    CPPUNIT_ASSERT(exps[1]->format() == "Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("hey there; how are you; ", exps));
    CPPUNIT_ASSERT(exps.size() == 2);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(hey, user_provided, unscoped)), Trailers()), Term(Value(Variable(there, user_provided, unscoped)), Trailers()))");
    CPPUNIT_ASSERT(exps[1]->format() == "Application(Term(Value(Variable(how, user_provided, unscoped)), Trailers()), Term(Value(Variable(are, user_provided, unscoped)), Trailers()), Term(Value(Variable(you, user_provided, unscoped)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("hey there\nhow are you\n", exps));
    CPPUNIT_ASSERT(exps.size() == 2);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(Variable(hey, user_provided, unscoped)), Trailers()), Term(Value(Variable(there, user_provided, unscoped)), Trailers()))");
    CPPUNIT_ASSERT(exps[1]->format() == "Application(Term(Value(Variable(how, user_provided, unscoped)), Trailers()), Term(Value(Variable(are, user_provided, unscoped)), Trailers()), Term(Value(Variable(you, user_provided, unscoped)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(f\nf)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(SubExpression(Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers()), Term(Value(Variable(f, user_provided, unscoped)), Trailers())))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(f;f)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0]->format() == "Application(Term(Value(SubExpression(Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers())), Application(Term(Value(Variable(f, user_provided, unscoped)), Trailers())))), Trailers()))");
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(ParserTest);

int main(int argc, char** argv) {
  CPPUNIT_NS::TextUi::TestRunner runner;
  runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
  runner.setOutputter(new CPPUNIT_NS::CompilerOutputter(&runner.result(),
      CPPUNIT_NS::stdCOut()));
  return runner.run() ? 0 : 1;
}
