#ifndef __COMMON_H__
#define __COMMON_H__

#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <stdexcept>

#define PTR boost::shared_ptr

namespace cirth {

  struct expectation_failure : std::runtime_error {
    expectation_failure(const std::string& msg_) throw ()
      : std::runtime_error(msg_) {}
    ~expectation_failure() throw() {}
  };

}

#endif
