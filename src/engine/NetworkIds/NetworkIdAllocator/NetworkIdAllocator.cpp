#include "NetworkIdAllocator.hpp"
#include <cstdint>
#include <limits>
#include "EngineException.hpp"

namespace rtype::engine {

NetworkIdAllocator::NetworkIdAllocator(std::uint32_t lastAllocated)
    : lastAllocated_(lastAllocated) {}

std::uint32_t NetworkIdAllocator::allocate() {
  if (lastAllocated_ == std::numeric_limits<std::uint32_t>::max()) {
    throw NetworkIdExhaustedException();
  }
  return ++lastAllocated_;
}

}  // namespace rtype::engine
