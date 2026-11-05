#include "HurtZoneSystem.hpp"
#include <algorithm>
#include "Bounds.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"
#include "HurtZone.hpp"
#include "Overlaps.hpp"
#include "Position.hpp"
#include "ShipBounds.hpp"
#include "ShipContact.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns Whether @p box hurts now, the enemy being in @p current: in the
 * phase it names, inside its window. */
bool isActive(const HurtBox& box, const CurrentAction* current) {
  if (!box.activeIn.has_value()) {
    return true;
  }
  if (current == nullptr || current->key != box.activeIn) {
    return false;
  }
  const HurtWindow& window = box.window;
  return current->since >= window.from &&
         (!window.until.has_value() || current->since < *window.until);
}

/** @returns @p box laid on the enemy standing at @p position. */
engine::Bounds hurtBoxBounds(const HurtBox& box, const Position& position) {
  return engine::Bounds{.centerX = position.x + box.offsetX,
                        .centerY = position.y + box.offsetY,
                        .width = box.width,
                        .height = box.height};
}

/** @returns The rectangle of a ship at @p position with the box @p box. */
engine::Bounds shipBoundsAt(const Position& position, const CollisionBox& box) {
  return engine::Bounds{.centerX = position.x,
                        .centerY = position.y,
                        .width = box.width,
                        .height = box.height};
}

/** @returns Whether an active box of @p zone, on the enemy at @p position that
 * is in @p current, overlaps @p ship. */
bool isHurt(const HurtZone& zone, const Position& position,
            const CurrentAction* current, const ShipBounds& ship) {
  return std::ranges::any_of(zone.boxes, [&](const HurtBox& box) noexcept {
    return isActive(box, current) &&
           engine::overlaps(hurtBoxBounds(box, position), ship.bounds);
  });
}

}  // namespace

HurtZoneSystem::HurtZoneSystem(engine::EventBus& events) : events_(events) {}

void HurtZoneSystem::collectShips(engine::ComponentRegistry& components) {
  ships_.clear();
  components.forEach<EntityType, CollisionBox>(
      [this, &components](engine::Entity entity, const EntityType& type,
                          const CollisionBox& box) {
        const Position* const position = components.get<Position>(entity);
        if (type == EntityType::Player && position != nullptr) {
          ships_.push_back(ShipBounds{.entity = entity,
                                      .bounds = shipBoundsAt(*position, box)});
        }
      });
}

void HurtZoneSystem::publishContacts(engine::ComponentRegistry& components) {
  components.forEach<HurtZone, Position>(
      [this, &components](engine::Entity enemy, const HurtZone& zone,
                          const Position& position) {
        const CurrentAction* const current =
            components.get<CurrentAction>(enemy);
        for (const ShipBounds& ship : ships_) {
          if (isHurt(zone, position, current, ship)) {
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
