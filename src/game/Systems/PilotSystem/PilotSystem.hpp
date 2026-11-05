#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Lets the behaviour of every body that has a Pilot, an Intent and a
 * Position write that Intent.
 *
 * It adds the elapsed time to the age of the Pilot first, so a behaviour
 * steers for the end of the tick. A body without a Pilot keeps its Intent:
 * the ship of a player is steered by its inputs. Run it before every system
 * that reads the Intent: MovementSystem and the weapon system.
 */
class PilotSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;
};

}  // namespace rtype::game
