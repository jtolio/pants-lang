#ifndef __COMPILE_H__
#define __COMPILE_H__

#include "common.h"
#include "cps.h"
#include "annotate.h"

namespace pants {
namespace compile {

  void compile(PTR<cps::Expression> cps, annotate::DataStore& store,
      std::ostream& os, bool use_gc);

}}

#endif
