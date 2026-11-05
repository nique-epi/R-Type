#include "BydomancerBehaviourFixtures.hpp"
#include <optional>
#include "BehaviourView.hpp"
#include "BydomancerBehaviourTestConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

BehaviourView bydomancerView(Position position, std::optional<Position> ship) {
  return BehaviourView{.position = position,
                       .origin = AT_ITS_COLUMN,
                       .age = BYDOMANCER_RESTED,
                       .elapsed = engine::SIMULATION_TICK_DURATION,
                       .topSpeed = BYDOMANCER_TOP_SPEED,
                       .settings = BYDOMANCER_SETTINGS_OF_THE_SHEET,
                       .action = std::nullopt,
                       .actionAge = BYDOMANCER_RESTED,
                       .nearestShip = ship};
}

}  // namespace rtype::game::testing
