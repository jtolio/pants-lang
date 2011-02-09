#ifndef __WRAP_H__
#define __WRAP_H__

#include "common.h"
#include "ast.h"

namespace cirth {
namespace wrap {

  void wrap(std::vector<PTR<cirth::ast::Expression> >& ast);
  
}}

#endif
