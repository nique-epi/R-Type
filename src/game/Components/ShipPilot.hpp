#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The player who flies a ship. A missile of that player never hits it,
 * even with friendly fire on; a missile of another player does then.
 */
struct ShipPilot {
  std::uint8_t playerNumber{0};
};

}  // namespace rtype::game
