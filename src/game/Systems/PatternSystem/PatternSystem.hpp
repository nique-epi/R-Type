#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Moves every entity that has a Position and a MovementPattern to the
 * height its pattern gives.
 *
 * Straight keeps the entity on its row. Wave puts it at originY plus
 * WAVE_AMPLITUDE * sin(2 * pi * age / WAVE_PERIOD_SECONDS), which is computed
 * from the age, never added up tick by tick, so the path does not depend on how
 * the simulation time is cut into steps. Run it before MovementSystem.
 */
class PatternSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;
};

}  // namespace rtype::game
