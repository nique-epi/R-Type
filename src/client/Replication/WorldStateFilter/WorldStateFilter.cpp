#include "WorldStateFilter.hpp"
#include <cstdint>

namespace rtype::client {

bool WorldStateFilter::accept(std::uint32_t tick) {
  if (lastAppliedTick_.has_value() && tick < *lastAppliedTick_) {
    return false;
  }
  lastAppliedTick_ = tick;
  return true;
}

}  // namespace rtype::client
