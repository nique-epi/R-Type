#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The gun of the ships of the players, as shipGunOf() reads it from
 * the sheets the session plays, so the client can fire its own missile before
 * the server answers: the type of the ships and of their missile, the ticks
 * between two shots and before the first one, and the velocity of the missile
 * in logical units per second.
 *
 * A projectileType of 0 names no missile the client may predict: the ships
 * fire nothing, or their missile does not fly in a straight line, and its
 * velocity is then 0 on both axes.
 */
struct ShipGun {
  std::uint8_t shipType{};
  std::uint8_t projectileType{};
  std::uint16_t intervalTicks{};
  std::uint16_t firstShotTicks{};
  float velocityX{};
  float velocityY{};

  bool operator==(const ShipGun&) const = default;
};

}  // namespace rtype::game
