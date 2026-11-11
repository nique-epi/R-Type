#pragma once

#include <cstddef>
#include <vector>
#include "CandidatePair.hpp"
#include "Collision.hpp"
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
 * left out. An entity with HitboxParts collides with each of those boxes in
 * place of its CollisionBox, and never with itself. Each pair of entities is
 * published once per update, however many of their boxes overlap, in no
 * particular order of the pairs or of the two entities. The boxes are first
 * sorted into an
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
  [[nodiscard]] bool touch(const engine::CandidatePair& pair) const;
  [[nodiscard]] bool hasSeveralBoxes(std::size_t candidate) const;
  [[nodiscard]] bool isPublished(const engine::Collision& contact) const;

  engine::EventBus& events_;
  /** @brief One entry per box, the boxes of one entity next to each other. */
  std::vector<EntityBounds> candidates_;
  /** @brief The contacts published on this update for an entity of several
   * boxes, so a second box of it publishes none again. */
  std::vector<engine::Collision> contactsOfParts_;
  engine::UniformGrid grid_;
};

}  // namespace rtype::game
