#pragma once

#include <vector>
#include "HurtBox.hpp"

namespace rtype::game {

/** @brief Component of an enemy: the boxes that hurt a ship on contact,
 * whatever its CollisionBox does. Declared by `[gameplay.hurt_zone]`;
 * HurtZoneSystem reads it. */
struct HurtZone {
  std::vector<HurtBox> boxes;

  bool operator==(const HurtZone&) const = default;
};

}  // namespace rtype::game
