#ifndef __WRAP_H__
#define __WRAP_H__

#include "common.h"
#include "ir.h"

namespace pants {
namespace wrap {

  void ir_prepend(std::vector<PTR<pants::ir::Expression> >& ir);
  void provided_names(std::set<ir::Name>& names);

}}

#endif
