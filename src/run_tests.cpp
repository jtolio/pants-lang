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
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}
  void testSimpleParse() {
    std::vector<PTR<cirth::ast::Expression> > exps;
    CPPUNIT_ASSERT(cirth::parser::parse("hey. there", exps));
    CPPUNIT_ASSERT(exps.size() == 1);
    CPPUNIT_ASSERT(exps[0].get());
    CPPUNIT_ASSERT(exps[0]->format() == "Application(FullValue(Variable(hey), Call()), FullValue(Variable(there)))");
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
