#pragma once

#include <vector>
#include "ISystem.hpp"
#include "ShipBounds.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Publishes a ShipContact for every ship that overlaps an active box of
 * the HurtZone of an enemy.
 *
 * A box is active when it has no `activeIn`, or when `activeIn` equals the
 * CurrentAction of the enemy. At most one ShipContact per ship and enemy and
 * update, however many boxes overlap. Only ships (EntityType::Player with a
 * Position and a CollisionBox) are hurt. What the contact costs is not decided
 * here: LifeKeeper does. The events are only queued: the loop must call
 * EventBus::dispatch() after the systems. The bus must outlive the system.
 */
class HurtZoneSystem final : public engine::ISystem {
 public:
  explicit HurtZoneSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  engine::EventBus& events_;
  std::vector<ShipBounds> ships_;
};

}  // namespace rtype::game
