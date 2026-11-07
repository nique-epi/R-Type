#include "HurtZoneSystem.hpp"
#include <algorithm>
#include "BoundsOf.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "HurtBox.hpp"
#include "HurtBoxBounds.hpp"
#include "HurtZone.hpp"
#include "HurtsNow.hpp"
#include "Overlaps.hpp"
#include "Position.hpp"
#include "ShipBounds.hpp"
#include "ShipContact.hpp"
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
          ships_.push_back(
              ShipBounds{.entity = entity, .bounds = boundsOf(*position, box)});
        }
      });
}

void HurtZoneSystem::publishContacts(engine::ComponentRegistry& components) {
  components.forEach<HurtZone, Position>([this, &components](
                                             engine::Entity enemy,
                                             const HurtZone& zone,
                                             const Position& position) {
    const CurrentAction* const current = components.get<CurrentAction>(enemy);
    for (const ShipBounds& ship : ships_) {
      if (std::ranges::any_of(zone.boxes, [current, &position,
                                           &ship](const HurtBox& box) {
            return hurtsNow(box, current) &&
                   engine::overlaps(hurtBoxBounds(box, position), ship.bounds);
          })) {
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
