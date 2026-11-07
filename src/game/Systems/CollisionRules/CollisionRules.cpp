#include "CollisionRules.hpp"
#include "Collision.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "MissileHit.hpp"
#include "MissileOwner.hpp"
#include "ShipContact.hpp"
#include "ShipPilot.hpp"

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
  const EntityType* firstType = components_.get<EntityType>(collision.first);
  const EntityType* secondType = components_.get<EntityType>(collision.second);
  if (firstType == nullptr || secondType == nullptr) {
    return;
  }
  publishIfRuleMatches(collision.first, *firstType, collision.second,
                       *secondType);
  publishIfRuleMatches(collision.second, *secondType, collision.first,
                       *firstType);
}

void CollisionRules::publishIfRuleMatches(engine::Entity first,
                                          EntityType firstType,
                                          engine::Entity second,
                                          EntityType secondType) {
  const bool playerMissileHitsEnemy =
      firstType == EntityType::PlayerMissile && secondType == EntityType::Enemy;
  const bool enemyMissileHitsPlayer =
      firstType == EntityType::EnemyMissile && secondType == EntityType::Player;
  const bool playerMissileHitsAnotherPilot =
      firstType == EntityType::PlayerMissile &&
      secondType == EntityType::Player &&
      hitsAnotherPilot(MissileHit{.missile = first, .target = second});
  if (playerMissileHitsEnemy || enemyMissileHitsPlayer ||
      playerMissileHitsAnotherPilot) {
    events_.publish(MissileHit{.missile = first, .target = second});
  } else if (firstType == EntityType::Player &&
             secondType == EntityType::Enemy) {
    events_.publish(ShipContact{.player = first, .enemy = second});
  }
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
