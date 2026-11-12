#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::game {

/** @brief What a body is told of a missile about to hit it: the network id of
 * the missile, 0 when it has none, and where the center of the missile lies
 * from the center of the body, in logical units. */
struct IncomingHit {
  std::uint32_t missileNetworkId{0};
  Position offset{};
};

}  // namespace rtype::game
