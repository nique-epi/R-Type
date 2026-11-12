#pragma once

#include <vector>
#include "EntityBounds.hpp"
#include "ISystem.hpp"
#include "TimeConstants.hpp"
#include "UniformGrid.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Publishes an engine::Collision for every pair of entities whose
 * collision boxes overlap and one of which hits a layer the other lies in.
 *
 * Only entities with a Position, a CollisionBox and an
 * engine::CollisionFilter are considered, and an entity still Summoning is
 * left out. Each pair is published once per update, in no particular order of
 * the pairs or of the two entities. The boxes are first sorted into an
 * engine::UniformGrid over the playfield, of cells of COLLISION_CELL_SIDE, and
 * only the pairs that share a cell are looked at: the cost grows with the
 * boxes that lie close to each other, not with the square of every box. Of
 * those, a pair engine::mayCollide() refuses is dropped before its boxes are
 * compared. The events are only queued: the loop must call
 * EventBus::dispatch() after the systems. The bus must outlive the system.
 */
class CollisionSystem final : public engine::ISystem {
 public:
  explicit CollisionSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  void gatherCandidates(engine::ComponentRegistry& components);
  void publishOverlaps();

  engine::EventBus& events_;
  std::vector<EntityBounds> candidates_;
  engine::UniformGrid grid_;
};

}  // namespace rtype::game
