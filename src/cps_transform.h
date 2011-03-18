#ifndef __CPS_TRANSFORM_H__
#define __CPS_TRANSFORM_H__

#include "common.h"
#include "ir.h"

namespace cirth {
namespace cps_transform {

  void transform(const std::vector<PTR<cirth::ir::Expression> >& in_ir,
      std::vector<PTR<cirth::ir::Expression> >& out_ir);

}}

#endif
