#ifndef __WRAP_H__
#define __WRAP_H__

#include "common.h"
#include "ir.h"

namespace cirth {
namespace wrap {

void ir_prepend(std::vector<PTR<cirth::ir::Expression> >& ir);

}}

#endif
