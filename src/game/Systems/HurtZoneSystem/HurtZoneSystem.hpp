#pragma once

#include <vector>
#include "BeamLine.hpp"
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
 * CurrentAction of the enemy; an enemy with no CurrentAction, or an idle one,
 * has only its boxes that hurt all the time. At most one ShipContact per ship
 * and enemy and update, however many boxes overlap. Only ships (EntityType
 * Player with a Position and a CollisionBox) are hurt. What the contact costs
 * is not decided here: LifeKeeper does. The events are only queued: the loop
 * must call EventBus::dispatch() after the systems. The bus must outlive the
 * system.
 */
class HurtZoneSystem final : public engine::ISystem {
 public:
  explicit HurtZoneSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  /** @brief Lists in ships_ the rectangle of every ship of @p components. */
  void collectShips(engine::ComponentRegistry& components);

  /** @brief Publishes the ShipContact of every ship of ships_ that an enemy of
   * @p components hurts. */
  void publishContacts(engine::ComponentRegistry& components);

  engine::EventBus& events_;
  std::vector<ShipBounds> ships_;
  std::vector<BeamLine> beamLines_;
};

}  // namespace rtype::game
