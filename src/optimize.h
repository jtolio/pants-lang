#ifndef __OPTIMIZE_H__
#define __OPTIMIZE_H__

#include "common.h"
#include "ir.h"
#include "cps.h"

namespace cirth {
namespace optimize {

  void ir(std::vector<PTR<cirth::ir::Expression> >& ir);
  void cps(PTR<cirth::cps::Expression>& cps);

}}

#endif
