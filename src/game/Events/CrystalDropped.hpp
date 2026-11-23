#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Event published when a missile kills an enemy that carries crystals:
 * the sheet of the crystal it drops, and where it died. Whoever makes the
 * crystal appear does it once the tick is over, never inside the query that
 * found the enemy.
 */
struct CrystalDropped {
  std::uint8_t sheetNumber{0};
  Position position{};
};

}  // namespace rtype::game
