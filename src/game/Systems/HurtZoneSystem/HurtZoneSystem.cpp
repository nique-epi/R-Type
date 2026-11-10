#include "HurtZoneSystem.hpp"
#include "ActiveHurtsLaying.hpp"
#include "BoundsOf.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntityBounds.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "HurtZone.hpp"
#include "HurtingEnemy.hpp"
#include "Position.hpp"
#include "ShipContact.hpp"
#include "Summoning.hpp"
#include "TimeConstants.hpp"
#include "ViewDelay.hpp"

namespace rtype::game {

HurtZoneSystem::HurtZoneSystem(engine::EventBus& events) : events_(events) {}

void HurtZoneSystem::collectShips(engine::ComponentRegistry& components) {
  ships_.clear();
  components.forEach<EntityType, CollisionBox>(
      [this, &components](engine::Entity entity, const EntityType& type,
                          const CollisionBox& box) {
        const Position* const position = components.get<Position>(entity);
        if (type == EntityType::Player && position != nullptr &&
            !components.has<ViewDelay>(entity)) {
          ships_.push_back(EntityBounds{.entity = entity,
                                        .bounds = boundsOf(*position, box)});
        }
      });
}

void HurtZoneSystem::publishContacts(engine::ComponentRegistry& components) {
  components.forEach<HurtZone, Position>(
      [this, &components](engine::Entity enemy, const HurtZone& zone,
                          const Position& position) {
        if (components.has<Summoning>(enemy)) {
          return;
        }
        const HurtingEnemy hurting{
            .zone = zone,
            .position = position,
            .current = components.get<CurrentAction>(enemy)};
        layActiveHurts(hurting, hurts_);
        for (const EntityBounds& ship : ships_) {
          if (hurtsBounds(hurts_, ship.bounds)) {
            events_.publish(ShipContact{.player = ship.entity, .enemy = enemy});
          }
        }
      });
}

void HurtZoneSystem::update(engine::ComponentRegistry& components,
                            [[maybe_unused]] engine::Duration elapsed) {
  collectShips(components);
  if (!ships_.empty()) {
    publishContacts(components);
  }
}

}  // namespace rtype::game
