#pragma once

#include "EntityType.hpp"

namespace rtype::game {

/** @returns Whether @p side is the side of a missile, of a player or of an
 * enemy. */
constexpr bool isAMissile(EntityType side) {
  return side == EntityType::PlayerMissile || side == EntityType::EnemyMissile;
}

}  // namespace rtype::game
