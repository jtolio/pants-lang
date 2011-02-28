#ifndef __WRAP_H__
#define __WRAP_H__

#include "common.h"
#include "pre_cps_ir.h"

namespace cirth {
namespace wrap {

  void wrap(std::vector<PTR<cirth::pre_cps_ir::Expression> >& ir);

}}

#endif
