#pragma once

#include <cstdint>
#include "ShotPattern.hpp"

namespace rtype::game {

/**
 * @brief A gun of the ships of the players, as shipArsenalOf() reads it from
 * the sheets the session plays, so the client can fire its own missiles
 * before the server answers: the type of the ships and of their missile, the
 * ticks between two shots and before the first one, how the missiles of a
 * shot are laid, and how they fly, in logical units per second: at the
 * velocity of the gun for a missile that flies straight, or at scatterSpeed
 * away from the muzzle, along the heading the shot gave it, for a missile
 * that scatters.
 *
 * A projectileType of 0 names no missile the client may predict: the ships
 * fire nothing, or their missile flies on another path, and its velocity and
 * its scatterSpeed are then 0.
 */
struct ShipGun {
  std::uint8_t shipType{};
  std::uint8_t projectileType{};
  std::uint16_t intervalTicks{};
  std::uint16_t firstShotTicks{};
  float velocityX{};
  float velocityY{};
  ShotPattern pattern{};
  float scatterSpeed{};

  bool operator==(const ShipGun&) const = default;
};

}  // namespace rtype::game
