#pragma once

#include <vector>
#include "Bounds.hpp"
#include "Entity.hpp"
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Publishes an engine::Collision for every pair of entities whose
 * collision boxes overlap.
 *
 * Only entities with a Position and a CollisionBox are considered. Each pair
 * is published once per update, in no particular order of the two entities.
 * Every pair is tested, so the cost grows with the square of the number of
 * boxes. The events are only queued: the loop must call EventBus::dispatch()
 * after the systems. The bus must outlive the system.
 */
class CollisionSystem final : public engine::ISystem {
 public:
  explicit CollisionSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  struct Candidate {
    engine::Entity entity;
    engine::Bounds bounds;
  };

  engine::EventBus& events_;
  std::vector<Candidate> candidates_;
};

}  // namespace rtype::game
