#pragma once

#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where a missile fired by the ship centered on @p ship appears: on the
 * row of the ship, its back against the front of the ship, so it leaves from
 * the nose of the ship.
 */
[[nodiscard]] Position missileStartPosition(Position ship);

}  // namespace rtype::game
