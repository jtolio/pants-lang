#include "optimize.h"

using namespace cirth;

void cirth::optimize::ir(std::vector<PTR<ir::Expression> >& ir) {
  // TODO: here, we want to group as many definitions together as we can
}

void cirth::optimize::cps(PTR<cps::Expression>& cps) {
  // TODO: here, we want to compact immediate calls to continuations to define
  //  variables into fewer calls
}
