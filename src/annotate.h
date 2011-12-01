#ifndef __ANNOTATE_H__
#define __ANNOTATE_H__

#include "common.h"
#include "cps.h"

namespace pants {
namespace annotate {

  class DataStore : boost::noncopyable {
    public:
      DataStore() {}
      bool isMutated(unsigned int varid) {
        std::map<unsigned int, bool>::const_iterator it(m_mutability.find(varid));
        if(it == m_mutability.end()) return false;
        return it->second;
      }
      void setMutated(unsigned int varid) {
        m_mutability[varid] = true;
      }
    private:
      std::map<unsigned int, bool> m_mutability;
  };

  void varids(PTR<pants::cps::Expression>& cps, DataStore& store);

}}

#endif
