#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The path an entity follows on top of its Velocity.
 */
enum class PatternShape : std::uint8_t {
  Straight,
  Wave,
};

/**
 * @brief Makes an entity follow a PatternShape around the row it started on.
 *
 * PatternSystem sets the height of the entity to originY plus the offset the
 * shape gives at ageSeconds, the simulation time since the entity appeared.
 * The horizontal movement still comes from the Velocity.
 */
struct MovementPattern {
  PatternShape shape{PatternShape::Straight};
  float originY{0.0F};
  float ageSeconds{0.0F};
};

}  // namespace rtype::game
