#include "ArrivalReveal.hpp"
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "AwaitedArrival.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityCues.hpp"
#include "EntityFeedback.hpp"
#include "InterpolationClock.hpp"
#include "PositionHistory.hpp"
#include "Sprite.hpp"

namespace rtype::client {

namespace {

/** @returns The entities awaiting their arrival whose oldest state is at or
 * before @p tick. */
std::vector<engine::Entity> arrivedBy(double tick,
                                      engine::ComponentRegistry& components) {
  std::vector<engine::Entity> arrived;
  components.forEach<AwaitedArrival, PositionHistory>(
      [tick, &arrived](engine::Entity entity, const AwaitedArrival&,
                       const PositionHistory& history) {
        const std::optional<std::uint32_t> oldest = history.oldestTick();
        if (oldest.has_value() && static_cast<double>(*oldest) <= tick) {
          arrived.push_back(entity);
        }
      });
  return arrived;
}

/** @brief Gives @p entity back the sprite its AwaitedArrival held. */
void giveBackTheSprite(engine::ComponentRegistry& components,
                       engine::Entity entity) {
  auto* const awaited = components.get<AwaitedArrival>(entity);
  if (awaited == nullptr) {
    return;
  }
  game::Sprite sprite = std::move(awaited->sprite);
  components.remove<AwaitedArrival>(entity);
  components.add(entity, std::move(sprite));
}

}  // namespace

void revealArrivedEntities(const InterpolationClock& clock,
                           engine::ComponentRegistry& components,
                           const EntityFeedback& feedback) {
  const std::optional<double> shown = clock.renderTick();
  if (!shown.has_value()) {
    return;
  }
  for (const engine::Entity entity : arrivedBy(*shown, components)) {
    giveBackTheSprite(components, entity);
    announce(feedback, components, entity);
  }
}

}  // namespace rtype::client
