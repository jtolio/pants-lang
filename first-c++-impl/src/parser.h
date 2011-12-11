#ifndef __PARSER_H__
#define __PARSER_H__

#include "common.h"
#include "ast.h"

namespace pants {
namespace parser {

  bool parse(const std::string& src,
      std::vector<PTR<pants::ast::Expression> >& ast);

}}

#endif
