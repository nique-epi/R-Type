#include "DrillbackBehaviourFixtures.hpp"
#include <optional>
#include "BehaviourView.hpp"
#include "DrillbackBehaviourTestConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

BehaviourView drillbackView(Position position) {
  return BehaviourView{.position = position,
                       .origin = position,
                       .age = DRILLBACK_RESTED,
                       .elapsed = engine::SIMULATION_TICK_DURATION,
                       .topSpeed = DRILLBACK_TOP_SPEED,
                       .settings = DRILLBACK_SETTINGS_OF_THE_SHEET,
                       .action = std::nullopt,
                       .actionAge = DRILLBACK_RESTED};
}

}  // namespace rtype::game::testing
