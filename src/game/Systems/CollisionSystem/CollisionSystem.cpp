#include "CollisionSystem.hpp"
#include <algorithm>
#include <cstddef>
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CandidatePair.hpp"
#include "CollidingBoxes.hpp"
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionFilter.hpp"
#include "CollisionFiltering.hpp"
#include "CollisionSystemConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityBounds.hpp"
#include "EventBus.hpp"
#include "HitboxParts.hpp"
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
  contactsOfParts_.clear();
  grid_.clear();
  components.forEach<Position, CollisionBox>(
      [this, &components](engine::Entity entity, const Position& position,
                          const CollisionBox& box) {
        const auto* const filter =
            components.get<engine::CollisionFilter>(entity);
        if (filter == nullptr || components.has<Summoning>(entity)) {
          return;
        }
        for (const CollisionBox& part :
             collidingBoxesOf(box, components.get<HitboxParts>(entity))) {
          const engine::Bounds bounds = boundsOf(position, part);
          candidates_.push_back(EntityBounds{
              .entity = entity, .bounds = bounds, .filter = *filter});
          grid_.insert(bounds);
        }
      });
}

void CollisionSystem::publishOverlaps() {
  for (const engine::CandidatePair& pair : grid_.candidatePairs()) {
    if (!touch(pair)) {
      continue;
    }
    const engine::Collision contact{.first = candidates_[pair.first].entity,
                                    .second = candidates_[pair.second].entity};
    if (hasSeveralBoxes(pair.first) || hasSeveralBoxes(pair.second)) {
      if (isPublished(contact)) {
        continue;
      }
      contactsOfParts_.push_back(contact);
    }
    events_.publish(contact);
  }
}

bool CollisionSystem::touch(const engine::CandidatePair& pair) const {
  const EntityBounds& first = candidates_[pair.first];
  const EntityBounds& second = candidates_[pair.second];
  return first.entity != second.entity &&
         engine::mayCollide(first.filter, second.filter) &&
         engine::overlaps(first.bounds, second.bounds);
}

bool CollisionSystem::hasSeveralBoxes(std::size_t candidate) const {
  const engine::Entity entity = candidates_[candidate].entity;
  return (candidate > 0 && candidates_[candidate - 1].entity == entity) ||
         (candidate + 1 < candidates_.size() &&
          candidates_[candidate + 1].entity == entity);
}

bool CollisionSystem::isPublished(const engine::Collision& contact) const {
  return std::ranges::find(contactsOfParts_, contact) != contactsOfParts_.end();
}

}  // namespace rtype::game
