#include "BlockingObstacles.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Obstacle.hpp"
#include "ObstacleContact.hpp"
#include "Position.hpp"
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
  boxes.clear();
  components.forEach<game::Obstacle, game::Position>(
      [&components, &boxes, lead](engine::Entity entity,
                                  const game::Obstacle& obstacle,
                                  const game::Position& position) {
        const auto* const box = components.get<game::CollisionBox>(entity);
        if (obstacle.contact != game::ObstacleContact::Blocking ||
            box == nullptr) {
          return;
        }
        engine::Bounds bounds = game::boundsOf(position, *box);
        bounds.centerX -= lead;
        boxes.push_back(bounds);
      });
}

}  // namespace rtype::client
