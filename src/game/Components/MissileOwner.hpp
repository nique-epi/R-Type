#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The player who fired a missile, by player number, so that a kill is
 * credited to the right player.
 *
 * Only the missiles of players carry it; the missile of an enemy has no owner.
 */
struct MissileOwner {
  std::uint8_t playerNumber{0};
};

}  // namespace rtype::game
