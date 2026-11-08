#pragma once

#include <vector>
#include "HurtBeam.hpp"
#include "HurtBox.hpp"

namespace rtype::game {

/** @brief Component of an enemy: the boxes and the beams that hurt a ship
 * on contact, whatever its CollisionBox does. Declared by
 * `[gameplay.hurt_zone]`; HurtZoneSystem reads it. */
struct HurtZone {
  std::vector<HurtBox> boxes;
  /** @brief Its beams, `[[gameplay.hurt_zone.beam]]`; none for most enemies. */
  std::vector<HurtBeam> beams;

  bool operator==(const HurtZone&) const = default;
};

}  // namespace rtype::game
