#ifndef __COMPILE_H__
#define __COMPILE_H__

#include "common.h"
#include "cps.h"

namespace cirth {
namespace compile {

  void compile(PTR<cps::Expression> cps, std::ostream& os);

}}

#endif
