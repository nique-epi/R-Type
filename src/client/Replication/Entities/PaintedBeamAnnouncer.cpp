#include "PaintedBeamAnnouncer.hpp"
#include <optional>
#include "ActionKey.hpp"
#include "AnnouncedAction.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyAction.hpp"
#include "Entity.hpp"
#include "EntityWithAppearance.hpp"
#include "PaintedBeams.hpp"
#include "SheetPhase.hpp"

namespace rtype::client {

PaintedBeamAnnouncer::PaintedBeamAnnouncer(
    engine::ComponentRegistry& components)
    : components_(&components) {}

void PaintedBeamAnnouncer::followServerTick(std::optional<double> serverTick) {
  serverTick_ = serverTick;
}

void PaintedBeamAnnouncer::apply(const network::EnemyAction& action) {
  const std::optional<engine::Entity> enemy =
      entityWithAppearance(*components_, action.entityId);
  auto* const beams =
      enemy.has_value() ? components_->get<PaintedBeams>(*enemy) : nullptr;
  if (beams == nullptr) {
    return;
  }
  beams->announced.push_back(AnnouncedAction{
      .key = game::ActionKey{.attack = action.attack,
                             .phase = sheetPhaseOf(action.phase)},
      .serverTick = serverTick_.value_or(0.0)});
}

}  // namespace rtype::client
