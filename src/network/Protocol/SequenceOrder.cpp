#include "SequenceOrder.hpp"
#include <cstdint>

namespace rtype::network {

bool isNewerSequence(std::uint16_t candidate, std::uint16_t newest) {
  const auto distance = static_cast<std::uint16_t>(candidate - newest);
  return distance >= 1 && distance < SEQUENCE_HALF_RANGE;
}

}  // namespace rtype::network
