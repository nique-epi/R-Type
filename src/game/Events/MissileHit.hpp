#pragma once

#include "Entity.hpp"

namespace rtype::game {

/**
 * @brief Event published when a missile touches a target it can hurt: a
 * player missile an enemy, or an enemy missile a player.
 *
 * It only reports the contact. Taking health away and destroying the missile
 * belong to the damage rules.
 */
struct MissileHit {
  engine::Entity missile;
  engine::Entity target;

  bool operator==(const MissileHit&) const = default;
};

}  // namespace rtype::game
