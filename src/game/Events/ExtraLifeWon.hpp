#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when a player wins an extra life with its score. The
 * player number is the one of the protocol, 1 to 4.
 */
struct ExtraLifeWon {
  std::uint8_t playerNumber{};

  bool operator==(const ExtraLifeWon&) const = default;
};

}  // namespace rtype::game
