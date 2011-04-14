#include "compile.h"
#include "constants.h"
#include "wrap.h"

using namespace cirth::cps;

static void write_expression(PTR<Expression> cps, std::ostream& os) {

}

void cirth::compile::compile(PTR<Expression> cps, std::ostream& os) {

  std::vector<PTR<cps::Callable> > callables;
  std::set<Name> free_names;
  cps->callables(callables);
  cps->free_names(free_names);
  cirth::wrap::remove_provided_names(free_names);

  if(free_names.size() > 0)
    throw expectation_failure("unbound variable!");

  os << HEADER;

  for(unsigned int i = 0; i < callables.size(); ++i) {
    os << "struct env_" << callables[i]->c_name() << " {\n";
    std::set<Name> free_names;
    std::set<Name> arg_names;
    callables[i]->free_names(free_names);
    callables[i]->arg_names(arg_names);
    for(std::set<Name>::iterator it(free_names.begin()); it != free_names.end();
        ++it) {
      os << "\tunion Value " << it->c_name() << ";\n";
    }
    for(std::set<Name>::iterator it(arg_names.begin()); it != arg_names.end();
        ++it) {
      os << "\tunion Value " << it->c_name() << ";\n";
    }
    os << "};\n\n";

    os << "struct env_" << callables[i]->c_name() << "* alloc_env_"
       << callables[i]->c_name() << "(";
    for(std::set<Name>::iterator it(free_names.begin()); it != free_names.end();
        ++it) {
      if(it != free_names.begin()) os << ", ";
      os << "union Value " << it->c_name();
    }
    os << ") {\n\tstruct env_" << callables[i]->c_name()
       << "* t = GC_MALLOC(sizeof(struct env_" << callables[i]->c_name()
       << "));\n";
    for(std::set<Name>::iterator it(free_names.begin()); it != free_names.end();
        ++it) {
      os << "\tt->" << it->c_name() << " = " << it->c_name() << ";\n";
    }
    os << "\treturn t;\n}\n\n";
  }

  os << STARTMAIN;
  // os << GRANDCENTRAL;
  write_expression(cps, os);

  for(unsigned int i = 0; i < callables.size(); ++i) {
    os << callables[i]->c_name() << ":\n";
    write_expression(callables[i]->expression, os);
    // os << CALLFUNC;
  }

  // os << ENDMAIN;
}
