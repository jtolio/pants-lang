#include "cps.h"

using namespace cirth;

void cps::transform(
    const std::vector<PTR<ir::Expression> >& in_ir,
    std::vector<PTR<ir::Expression> >& out_ir) {
  std::vector<PTR<ir::Expression> >::const_iterator exp(in_ir.begin());
  // an expression can be:
  //  * Definition
  //  * Mutation
  //  * ReturnValue
  //  * ValueExpression (always last)
}
