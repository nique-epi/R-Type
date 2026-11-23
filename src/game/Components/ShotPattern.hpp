#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The shape of one shot of a gun: how many projectiles it fires at
 * once, the fan they spread over, in degrees, the rows between two of them,
 * and how far ahead of the muzzle they start, in logical units. The default
 * is one projectile, straight ahead, at the muzzle (see shotPlacement()).
 */
struct ShotPattern {
  std::uint8_t count{1};
  float spreadDegrees{0.0F};
  float stepY{0.0F};
  float distance{0.0F};

  bool operator==(const ShotPattern&) const = default;
};

}  // namespace rtype::game
