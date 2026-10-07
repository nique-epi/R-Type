#pragma once

#include "Entity.hpp"
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

/**
 * @brief Event published when two entities touch.
 */
struct Collision {
  rtype::engine::Entity first;
  rtype::engine::Entity second;

  bool operator==(const Collision&) const = default;
};

/**
 * @brief Event published when an entity is destroyed.
 */
struct EntityDestroyed {
  rtype::engine::Entity entity;
};

/**
 * @brief Component holding what an entity can still take before it dies.
 */
struct Health {
  int points;
};

/**
 * @brief Component of the entity a subscriber creates for a Collision.
 */
struct Explosion {
  rtype::engine::Entity source;
};

/**
 * @brief Publishes, during its forEach, a Collision between the missile and
 * every entity that has a Health, like a collision system would.
 */
class ContactSystem final : public rtype::engine::ISystem {
 public:
  ContactSystem(rtype::engine::EventBus& events, rtype::engine::Entity missile);

  void update(rtype::engine::ComponentRegistry& components,
              rtype::engine::Duration elapsed) override;

 private:
  rtype::engine::EventBus* events_;
  rtype::engine::Entity missile_;
};
