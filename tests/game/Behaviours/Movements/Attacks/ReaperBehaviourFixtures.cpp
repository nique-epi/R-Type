#include "ReaperBehaviourFixtures.hpp"
#include <optional>
#include "BehaviourView.hpp"
#include "Position.hpp"
#include "ReaperBehaviourTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

BehaviourView reaperView(Position position, std::optional<Position> ship) {
  return BehaviourView{.position = position,
                       .origin = position,
                       .age = RESTED,
                       .elapsed = engine::SIMULATION_TICK_DURATION,
                       .topSpeed = REAPER_TOP_SPEED,
                       .settings = REAPER_SETTINGS_OF_THE_SHEET,
                       .action = std::nullopt,
                       .actionAge = RESTED,
                       .nearestShip = ship};
}

}  // namespace rtype::game::testing
