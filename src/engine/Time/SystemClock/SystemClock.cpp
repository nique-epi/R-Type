#include "SystemClock.hpp"
#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::engine {

Duration SystemClock::now() const {
  return std::chrono::duration_cast<Duration>(
      std::chrono::steady_clock::now().time_since_epoch());
}

}  // namespace rtype::engine
