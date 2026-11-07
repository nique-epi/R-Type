#include "PositionInterpolation.hpp"
#include <optional>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "InterpolationClock.hpp"
#include "Position.hpp"
#include "PositionHistory.hpp"

namespace rtype::client {

void placeInterpolatedPositions(const InterpolationClock& clock,
                                engine::ComponentRegistry& components) {
  const std::optional<double> shown = clock.renderTick();
  if (!shown.has_value()) {
    return;
  }
  const double tick = *shown;
  components.forEach<game::Position, PositionHistory>(
      [tick](engine::Entity, game::Position& position,
             PositionHistory& history) {
        if (const std::optional<game::Position> where = history.at(tick)) {
          position = *where;
        }
      });
}

}  // namespace rtype::client
