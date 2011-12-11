#ifndef __OPTIMIZE_H__
#define __OPTIMIZE_H__

#include "common.h"
#include "ir.h"
#include "cps.h"
#include "annotate.h"

namespace pants {
namespace optimize {

  void ir(std::vector<PTR<pants::ir::Expression> >& ir);
  void cps(PTR<pants::cps::Expression>& cps, pants::annotate::DataStore& store);

}}

#endif
