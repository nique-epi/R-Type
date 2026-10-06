#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief What an entity is in the match.
 *
 * Attached as a component like Position or Velocity. A missile records the
 * side that fired it, because a player missile and an enemy missile do not
 * hit the same targets.
 */
enum class EntityType : std::uint8_t {
  Player,
  Enemy,
  PlayerMissile,
  EnemyMissile,
};

}  // namespace rtype::game
