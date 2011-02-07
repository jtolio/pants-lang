#ifndef __PARSER_H__
#define __PARSER_H__

#include "common.h"
#include "ast.h"

namespace cirth {
namespace parser {

  bool parse(const std::string& src,
      std::vector<PTR<cirth::ast::Expression> >& ast);
  
}}

#endif
