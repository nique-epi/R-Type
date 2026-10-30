#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Moves every entity that has a Position and a Velocity, and every
 * entity that has a Position, a ShipMotion and an Intent.
 *
 * The distance of a Velocity is the velocity, in logical units per second,
 * multiplied by the elapsed simulation time: it depends on the time given,
 * never on how many frames were rendered. Entities without a Velocity do not
 * move that way, and are not kept inside the playfield.
 *
 * An entity with a ShipMotion moves by the directions its Intent holds, as
 * shipAfter() computes it, whichever controller wrote the Intent; it is kept
 * inside the playfield.
 */
class MovementSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;
};

}  // namespace rtype::game
