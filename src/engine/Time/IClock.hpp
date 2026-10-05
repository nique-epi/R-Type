#pragma once

#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief Source of monotonic time, measured from an arbitrary fixed origin.
 */
class IClock {
 public:
  virtual ~IClock() = default;
  virtual Duration now() const = 0;
};

}  // namespace rtype::engine
