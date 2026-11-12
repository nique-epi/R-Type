#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief What an entity is in the match.
 *
 * Attached as a component like Position or CollisionBox. A missile records the
 * side that fired it, because a player missile and an enemy missile do not
 * hit the same targets. A look item is the item of the locker a ship picks up.
 * An obstacle is a wall or a piece of the decor: it stops what hits it, and
 * blocks or kills a ship that touches it (Obstacle).
 */
enum class EntityType : std::uint8_t {
  Player,
  Enemy,
  PlayerMissile,
  EnemyMissile,
  LookItem,
  Obstacle,
};

}  // namespace rtype::game
