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
  CPPUNIT_TEST(testMapVsIndex);
  CPPUNIT_TEST(testCallVsField);
  CPPUNIT_TEST(testSingleLists);
  CPPUNIT_TEST(testParses);
  CPPUNIT_TEST(testListExpansion);
  CPPUNIT_TEST(testByteString);
  CPPUNIT_TEST(testClosedCall);
  CPPUNIT_TEST(testFunctions);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}
  void testSimpleParse() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("hey' there", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(hey, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(there, user_provided)), Trailers()))");
  }
  
  void testMapVsIndex() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing [key: value]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(call, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(thing, user_provided)), Trailers()), FullValue(Headers(), Value(Map(MapDefinition(Application(FullValue(Headers(), Value(Variable(key, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(value, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing [key1: value1, key2: value2]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(call, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(thing, user_provided)), Trailers()), FullValue(Headers(), Value(Map(MapDefinition(Application(FullValue(Headers(), Value(Variable(key1, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(value1, user_provided)), Trailers()))), MapDefinition(Application(FullValue(Headers(), Value(Variable(key2, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(value2, user_provided)), Trailers()))))), Trailers()))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing[key]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(call, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(thing, user_provided)), Trailers(Index(Application(FullValue(Headers(), Value(Variable(key, user_provided)), Trailers()))))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing[key1, key2]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(call, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(thing, user_provided)), Trailers(Index(List(Application(FullValue(Headers(), Value(Variable(key1, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(key2, user_provided)), Trailers())))))))");    
  }
  
  void testCallVsField() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing1' notafield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(call, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(thing1, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(notafield, user_provided)), Trailers()))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing1.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(call, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(thing1, user_provided)), Trailers(Field(Variable(afield, user_provided)))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' function'.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(call, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(function, user_provided)), Trailers(OpenCall(), Field(Variable(afield, user_provided)))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' function'.afuncfield'", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(call, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(function, user_provided)), Trailers(OpenCall(), Field(Variable(afuncfield, user_provided)), OpenCall())))");
  }
  
  void testClosedCall() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1, arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(ClosedCall(Application(FullValue(Headers(), Value(Variable(arg1, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(arg2, user_provided)), Trailers()))))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f (arg1, arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers()), FullValue(Headers(), Value(SubExpression(List(Application(FullValue(Headers(), Value(Variable(arg1, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(arg2, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f'(arg1, arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(OpenCall(), ClosedCall(Application(FullValue(Headers(), Value(Variable(arg1, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(arg2, user_provided)), Trailers()))))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f' (arg1, arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(SubExpression(List(Application(FullValue(Headers(), Value(Variable(arg1, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(arg2, user_provided)), Trailers()))))), Trailers()))");
  }
  
  void testSingleLists() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("z,", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "List(Application(FullValue(Headers(), Value(Variable(z, user_provided)), Trailers())))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("z, = 3", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Definition(AssigneeList(SingleAssignee(FullValue(Headers(), Value(Variable(z, user_provided)), Trailers()))), Application(FullValue(Headers(), Value(Integer(3)), Trailers())))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("[test:test,]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Map(MapDefinition(Application(FullValue(Headers(), Value(Variable(test, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(test, user_provided)), Trailers()))))), Trailers()))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1,)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(ClosedCall(Application(FullValue(Headers(), Value(Variable(arg1, user_provided)), Trailers()))))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(ClosedCall(Application(FullValue(Headers(), Value(Variable(arg1, user_provided)), Trailers()))))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f(arg1, arg2)", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(ClosedCall(Application(FullValue(Headers(), Value(Variable(arg1, user_provided)), Trailers())), Application(FullValue(Headers(), Value(Variable(arg2, user_provided)), Trailers()))))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,;| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(RequiredArgument(Variable(a, user_provided))), Optional()), Right(Required(), Optional()), Expressions(Application(FullValue(Headers(), Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|b,| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(RequiredArgument(Variable(b, user_provided))), Optional()), Expressions(Application(FullValue(Headers(), Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
  }
  
  void testFunctions() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("{}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a| print' \"hi\"; null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(RequiredArgument(Variable(a, user_provided))), Optional()), Expressions(Application(FullValue(Headers(), Value(Variable(print, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(CharString(hi)), Trailers())), Application(FullValue(Headers(), Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,| null}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(RequiredArgument(Variable(a, user_provided))), Optional()), Expressions(Application(FullValue(Headers(), Value(Variable(null, user_provided)), Trailers()))))), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional(OptionalArgument(Variable(c, user_provided), FullValue(Headers(), Value(Integer(3)), Trailers())))), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3,d:4|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional(OptionalArgument(Variable(c, user_provided), FullValue(Headers(), Value(Integer(3)), Trailers())), OptionalArgument(Variable(d, user_provided), FullValue(Headers(), Value(Integer(4)), Trailers())))), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,c:3,d:4,*(remainder)|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional(OptionalArgument(Variable(c, user_provided), FullValue(Headers(), Value(Integer(3)), Trailers())), OptionalArgument(Variable(d, user_provided), FullValue(Headers(), Value(Integer(4)), Trailers()))), Variadic(Variable(remainder, user_provided))), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(!cirth::parser::parse("{|a,b,c:3,d:4,q(remainder)|}", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{||}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|;|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional()), Right(Required(), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a;|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(RequiredArgument(Variable(a, user_provided))), Optional()), Right(Required(), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b;|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional()), Right(Required(), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|a,b,;|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional()), Right(Required(), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|c:3,a,b,;|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional(OptionalArgument(Variable(c, user_provided), FullValue(Headers(), Value(Integer(3)), Trailers())))), Right(Required(), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(var),c:3,a,b,;|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided))), Optional(OptionalArgument(Variable(c, user_provided), FullValue(Headers(), Value(Integer(3)), Trailers()))), Variadic(Variable(var, user_provided))), Right(Required(), Optional()), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(var),c:3,a,b,d;e,f,g:5,h:7,*(j)|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(RequiredArgument(Variable(a, user_provided)), RequiredArgument(Variable(b, user_provided)), RequiredArgument(Variable(d, user_provided))), Optional(OptionalArgument(Variable(c, user_provided), FullValue(Headers(), Value(Integer(3)), Trailers()))), Variadic(Variable(var, user_provided))), Right(Required(RequiredArgument(Variable(e, user_provided)), RequiredArgument(Variable(f, user_provided))), Optional(OptionalArgument(Variable(g, user_provided), FullValue(Headers(), Value(Integer(5)), Trailers())), OptionalArgument(Variable(h, user_provided), FullValue(Headers(), Value(Integer(7)), Trailers()))), Variadic(Variable(j, user_provided))), Expressions())), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("{|*(a);*(b)|}", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Function(Left(Required(), Optional(), Variadic(Variable(a, user_provided))), Right(Required(), Optional(), Variadic(Variable(b, user_provided))), Expressions())), Trailers()))");
/*  TODO: get failures to fail gracefully
    exps.clear();
    CPPUNIT_ASSERT(!cirth::parser::parse("{|c,*(a);|}", exps));
    exps.clear();
    CPPUNIT_ASSERT(!cirth::parser::parse("{|*(a),c|}", exps));
    exps.clear();
    CPPUNIT_ASSERT(!cirth::parser::parse("{|a:3,c|}", exps));
    exps.clear();
    CPPUNIT_ASSERT(!cirth::parser::parse("{|a,c:3;|}", exps));*/
  }
  
  void testListExpansion() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f' *(thing) thing", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(OpenCall())), ListExpansion(Application(FullValue(Headers(), Value(Variable(thing, user_provided)), Trailers()))), FullValue(Headers(), Value(Variable(thing, user_provided)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f' * (thing) thing", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(*, user_provided)), Trailers()), FullValue(Headers(), Value(SubExpression(Application(FullValue(Headers(), Value(Variable(thing, user_provided)), Trailers())))), Trailers()), FullValue(Headers(), Value(Variable(thing, user_provided)), Trailers()))");
  }
  
  void testByteString() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("f' b\"thing\"", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(ByteString(thing)), Trailers()))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("f' b \"thing\"", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Headers(), Value(Variable(f, user_provided)), Trailers(OpenCall())), FullValue(Headers(), Value(Variable(b, user_provided)), Trailers()), FullValue(Headers(), Value(CharString(thing)), Trailers()))");
  }
  
  void testParses() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("x = 1", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("x, = 1,", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("x, y = 1, 2", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("x, y, = 1, 2,", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("x,", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("x, y", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("x, y,", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(a b).thing", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(a b).thing = 3", exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("(a b).thing, (a b)[\"thing\"] = 3, 4",
        exps));
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("z.thing = 3; x[4] = 5", exps));
    exps.clear();
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
