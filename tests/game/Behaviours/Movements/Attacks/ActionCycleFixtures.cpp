#include "ActionCycleFixtures.hpp"
#include <optional>
#include "ActionKey.hpp"
#include "BehaviourView.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

BehaviourView viewIn(std::optional<ActionKey> action, engine::Duration age) {
  return BehaviourView{.position = {},
                       .origin = {},
                       .age = age,
                       .elapsed = engine::SIMULATION_TICK_DURATION,
                       .topSpeed = 0.0F,
                       .settings = {},
                       .action = action,
                       .actionAge = age};
}

}  // namespace rtype::game::testing
