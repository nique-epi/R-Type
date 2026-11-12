#include "BlockingObstacles.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>
#include "Bounds.hpp"
#include "ComponentRegistry.hpp"
#include "ObstacleBlocking.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

float scrollLeadOf(float scrollSpeed, std::optional<double> serverTick,
                   std::uint32_t shownTick) {
  if (!serverTick.has_value()) {
    return 0.0F;
  }
  const double ticks =
      std::max(0.0, *serverTick - static_cast<double>(shownTick));
  return scrollSpeed *
         static_cast<float>(
             ticks / static_cast<double>(engine::SIMULATION_TICKS_PER_SECOND));
}

void collectBlockingObstacles(engine::ComponentRegistry& components, float lead,
                              std::vector<engine::Bounds>& boxes) {
  game::collectBlockingBoxes(components, boxes);
  for (engine::Bounds& box : boxes) {
    box.centerX -= lead;
  }
}

}  // namespace rtype::client
