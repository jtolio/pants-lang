#include "cps.h"

using namespace cirth;

void cps::transform(const std::vector<PTR<ir::Assignment> >& in_ir,
    const PTR<ir::Value>& in_lastval, PTR<cps::Expression>& out_ir) {
  PTR<cps::Value> continuation;
  PTR<cps::Value> exception;
  
  for(unsigned int i = 0; i < in_ir.size(); ++i) {
    
  }
}
