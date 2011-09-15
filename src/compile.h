#ifndef __COMPILE_H__
#define __COMPILE_H__

#include "common.h"
#include "cps.h"

namespace pants {
namespace compile {

  void compile(PTR<cps::Expression> cps, std::ostream& os, bool use_gc);

}}

#endif
