#include "CollisionSystem.hpp"
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CandidatePair.hpp"
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionFilter.hpp"
#include "CollisionFiltering.hpp"
#include "CollisionSystemConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityBounds.hpp"
#include "EventBus.hpp"
#include "Overlaps.hpp"
#include "Position.hpp"
#include "Summoning.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

CollisionSystem::CollisionSystem(engine::EventBus& events)
    : events_(events), grid_(COLLISION_GRID_AREA, COLLISION_CELL_SIDE) {}

void CollisionSystem::update(engine::ComponentRegistry& components,
                             [[maybe_unused]] engine::Duration elapsed) {
  gatherCandidates(components);
  publishOverlaps();
}

void CollisionSystem::gatherCandidates(engine::ComponentRegistry& components) {
  candidates_.clear();
  grid_.clear();
  components.forEach<Position, CollisionBox>(
      [this, &components](engine::Entity entity, const Position& position,
                          const CollisionBox& box) {
        const auto* const filter =
            components.get<engine::CollisionFilter>(entity);
        if (filter == nullptr || components.has<Summoning>(entity)) {
          return;
        }
        const engine::Bounds bounds = boundsOf(position, box);
        candidates_.push_back(EntityBounds{
            .entity = entity, .bounds = bounds, .filter = *filter});
        grid_.insert(bounds);
      });
}

void CollisionSystem::publishOverlaps() {
  for (const engine::CandidatePair& pair : grid_.candidatePairs()) {
    const EntityBounds& first = candidates_[pair.first];
    const EntityBounds& second = candidates_[pair.second];
    if (engine::mayCollide(first.filter, second.filter) &&
        engine::overlaps(first.bounds, second.bounds)) {
      events_.publish(
          engine::Collision{.first = first.entity, .second = second.entity});
    }
  }
}

}  // namespace rtype::game
