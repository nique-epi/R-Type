#include "CollisionRules.hpp"
#include <optional>
#include "CollidingEntity.hpp"
#include "Collision.hpp"
#include "CollisionFilter.hpp"
#include "CollisionFiltering.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "IsAMissile.hpp"
#include "LookItemTouched.hpp"
#include "MissileHit.hpp"
#include "MissileOwner.hpp"
#include "ShipContact.hpp"
#include "ShipPilot.hpp"
#include "ViewDelay.hpp"

namespace rtype::game {

CollisionRules::CollisionRules(engine::EventBus& events,
                               engine::ComponentRegistry& components,
                               bool friendlyFire)
    : events_(events),
      components_(components),
      friendlyFire_(friendlyFire),
      subscription_(events.subscribe<engine::Collision>(
          [this](const engine::Collision& collision) {
            onCollision(collision);
          })) {}

CollisionRules::~CollisionRules() { events_.unsubscribe(subscription_); }

void CollisionRules::onCollision(const engine::Collision& collision) {
  const std::optional<CollidingEntity> first =
      collidingEntityOf(collision.first);
  const std::optional<CollidingEntity> second =
      collidingEntityOf(collision.second);
  if (!first.has_value() || !second.has_value() ||
      !engine::mayCollide(first->filter, second->filter)) {
    return;
  }
  publishContactOf(*first, *second);
  publishContactOf(*second, *first);
}

std::optional<CollidingEntity> CollisionRules::collidingEntityOf(
    engine::Entity entity) const {
  const auto* const type = components_.get<EntityType>(entity);
  const auto* const filter = components_.get<engine::CollisionFilter>(entity);
  if (type == nullptr || filter == nullptr) {
    return std::nullopt;
  }
  return CollidingEntity{.entity = entity, .type = *type, .filter = *filter};
}

void CollisionRules::publishContactOf(CollidingEntity one,
                                      CollidingEntity other) {
  if (isAMissile(one.type)) {
    if (isAHitOf(one, other)) {
      events_.publish(
          MissileHit{.missile = one.entity, .target = other.entity});
    }
  } else if (one.type == EntityType::Player &&
             other.type == EntityType::Enemy &&
             !components_.has<ViewDelay>(one.entity)) {
    events_.publish(ShipContact{.player = one.entity, .enemy = other.entity});
  } else if (one.type == EntityType::Player &&
             other.type == EntityType::LookItem) {
    events_.publish(LookItemTouched{.ship = one.entity, .item = other.entity});
  }
}

bool CollisionRules::isAHitOf(CollidingEntity missile,
                              CollidingEntity target) const {
  if (isAMissile(target.type)) {
    return engine::hitsLayerOf(missile.filter, target.filter);
  }
  if (missile.type == EntityType::PlayerMissile &&
      target.type == EntityType::Player) {
    return hitsAnotherPilot(
        MissileHit{.missile = missile.entity, .target = target.entity});
  }
  const bool testedWhereItsPlayerSaw =
      (missile.type == EntityType::PlayerMissile &&
       target.type == EntityType::Enemy &&
       components_.has<ViewDelay>(missile.entity)) ||
      (missile.type == EntityType::EnemyMissile &&
       target.type == EntityType::Player &&
       components_.has<ViewDelay>(target.entity));
  return !testedWhereItsPlayerSaw;
}

bool CollisionRules::hitsAnotherPilot(const MissileHit& hit) const {
  if (!friendlyFire_) {
    return false;
  }
  const auto* const owner = components_.get<MissileOwner>(hit.missile);
  const auto* const pilot = components_.get<ShipPilot>(hit.target);
  return owner != nullptr && pilot != nullptr &&
         owner->playerNumber != pilot->playerNumber;
}

}  // namespace rtype::game
