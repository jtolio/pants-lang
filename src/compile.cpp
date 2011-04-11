#include "compile.h"
#include "constants.h"
#include "wrap.h"

using namespace cirth::cps;

void cirth::compile::compile(PTR<Expression> cps, std::ostream& os) {

  std::vector<PTR<cps::Callable> > callables;
  std::set<cps::Name> free_names;
  cps->callables(callables);
  cps->free_names(free_names);
  cirth::wrap::remove_provided_names(free_names);

  if(free_names.size() > 0)
    throw expectation_failure("unbound variable!");
  
  os << HEADER;
/*  
  for(unsigned int i = 0; i < callables.size(); ++i) {
    os << "struct env_" << functionname << " {\n";
    for arg in args + freevars
      os << "\tunion Value " << varname << ";\n";
    os << "};\n\n";
    
    os << "struct env_" << functionname << "* alloc_env_" << functionname
       << "(";
    for arg in freevars
      if i > 0 os << ", ";
      os << "union Value " << varname
    os << ") {\n\tstruct env_" << functionname
       << "* t = GC_MALLOC(sizeof(struct env_" << functionname << "));\n";
    for arg in freevars
      os << "\tt->" << varname << " = " << varname << ";\n";
    os << "\treturn t;\n}\n\n";
  }
  
  os << STARTMAIN;
  
  os << "\t 
  */
}
