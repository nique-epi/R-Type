#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Moves every entity that has a Position and a Velocity.
 *
 * The distance is the velocity, in logical units per second, multiplied by the
 * elapsed simulation time: it depends on the time given, never on how many
 * frames were rendered. Entities without a Velocity do not move. Entities are
 * not kept inside the playfield.
 */
class MovementSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;
};

}  // namespace rtype::game
