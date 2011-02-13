#include "wrap.h"

using namespace cirth::ast;

static void bindTopLevelNames(std::vector<PTR<Expression> >&);
static void addReturns(std::vector<PTR<Expression> >&);

void cirth::wrap::wrap(std::vector<PTR<Expression> >& ast) {
  addReturns(ast);
  bindTopLevelNames(ast);
}

static void bindTopLevelNames(std::vector<PTR<Expression> >& ast) {
  #define BIND_NAME(name) { \
    std::vector<PTR<Term> > terms; \
    terms.push_back(PTR<Term>(new FullValue(PTR<Value>(new Variable(name, \
        false)), std::vector<PTR<Trailer> >()))); \
    ast.insert(ast.begin(), PTR<Expression>(new Definition(PTR<Assignee>( \
        new SingleAssignee(PTR<Term>(new FullValue(PTR<Value>(new Variable( \
        name, true)), std::vector<PTR<Trailer> >())))), PTR<Expression>(\
        new Application(terms))))); \
  }
  BIND_NAME("if");
  BIND_NAME("while");
  BIND_NAME("for");
  BIND_NAME("print");
  BIND_NAME("construct");
  BIND_NAME("import");
  BIND_NAME("+");
  BIND_NAME("-");
  BIND_NAME("/");
  BIND_NAME("*");
  BIND_NAME("and");
  BIND_NAME("or");
  BIND_NAME("not");
  BIND_NAME("true");
  BIND_NAME("false");
  BIND_NAME("assert");
  BIND_NAME("in");
  BIND_NAME("==");
  BIND_NAME("!=");
  BIND_NAME("null");
  BIND_NAME("inf");
  #undef BIND_NAME
}

static void addReturns(std::vector<PTR<Expression> >& ast) {

}
