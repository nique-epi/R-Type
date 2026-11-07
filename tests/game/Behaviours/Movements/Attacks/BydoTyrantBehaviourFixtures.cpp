#include "BydoTyrantBehaviourFixtures.hpp"
#include <optional>
#include "BehaviourView.hpp"
#include "BydoTyrantBehaviourTestConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

BehaviourView tyrantView(Position position, std::optional<Position> ship) {
  return BehaviourView{.position = position,
                       .origin = WHERE_THE_TYRANT_APPEARS,
                       .age = TYRANT_LONG_IN_PLAY,
                       .elapsed = engine::SIMULATION_TICK_DURATION,
                       .topSpeed = TYRANT_TOP_SPEED,
                       .settings = TYRANT_SETTINGS_OF_THE_SHEET,
                       .action = std::nullopt,
                       .actionAge = TYRANT_RESTED,
                       .nearestShip = ship,
                       .wounded = false,
                       .enraged = false,
                       .actionsEnded = ITS_ARRIVAL_ENDED};
}

}  // namespace rtype::game::testing
