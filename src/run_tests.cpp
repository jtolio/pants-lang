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
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}
  void testSimpleParse() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("hey' there", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(hey, "
        "user_provided), Call()), FullValue(Variable(there, user_provided)))");
  }
  
  void testMapVsIndex() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing [key: value]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(call, "
        "user_provided), Call()), FullValue(Variable(thing, user_provided)), "
        "FullValue(Map(MapDefinition(Application(FullValue(Variable(key, "
        "user_provided))), Application(FullValue(Variable(value, "
        "user_provided)))))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing [key1: value1, "
        "key2: value2]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(call, "
        "user_provided), Call()), FullValue(Variable(thing, user_provided)), "
        "FullValue(Map(MapDefinition(Application(FullValue(Variable(key1, "
        "user_provided))), Application(FullValue(Variable(value1, "
        "user_provided)))), MapDefinition(Application(FullValue(Variable(key2, "
        "user_provided))), Application(FullValue(Variable(value2, "
        "user_provided)))))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing[key]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(call, "
        "user_provided), Call()), FullValue(Variable(thing, user_provided), "
        "Index(Application(FullValue(Variable(key, user_provided))))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing[key1, key2]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(call, "
        "user_provided), Call()), FullValue(Variable(thing, user_provided), "
        "Index(List(Application(FullValue(Variable(key1, user_provided))), "
        "Application(FullValue(Variable(key2, user_provided)))))))");    
  }
  
  void testCallVsField() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing1' notafield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(call, "
        "user_provided), Call()), FullValue(Variable(thing1, user_provided), "
        "Call()), FullValue(Variable(notafield, user_provided)))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' thing1.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(call, "
        "user_provided), Call()), FullValue(Variable(thing1, user_provided), "
        "Field(Variable(afield, user_provided))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' function'.afield", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(call, "
        "user_provided), Call()), FullValue(Variable(function, user_provided), "
        "Call(), Field(Variable(afield, user_provided))))");    
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("call' function'.afuncfield'", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(call, "
        "user_provided), Call()), FullValue(Variable(function, user_provided), "
        "Call(), Field(Variable(afuncfield, user_provided)), Call()))");    
  }
  
  void testSingleLists() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("z,", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "List(Application(FullValue(Variable("
        "z, user_provided))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("z, = 3", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Definition(AssigneeList("
        "SingleAssignee(FullValue(Variable(z, user_provided)))), "
        "Application(FullValue(Integer(3))))");
    exps.clear();
    CPPUNIT_ASSERT(cirth::parser::parse("[test:test,]", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Map("
        "MapDefinition(Application(FullValue(Variable(test, user_provided))), "
        "Application(FullValue(Variable(test, user_provided)))))))");
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
