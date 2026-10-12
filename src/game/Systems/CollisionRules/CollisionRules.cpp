#include "CollisionRules.hpp"
#include "Collision.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "MissileHit.hpp"
#include "ShipContact.hpp"

namespace rtype::game {

CollisionRules::CollisionRules(engine::EventBus& events,
                               engine::ComponentRegistry& components)
    : events_(events),
      components_(components),
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
  if (playerMissileHitsEnemy || enemyMissileHitsPlayer) {
    events_.publish(MissileHit{.missile = first, .target = second});
  } else if (firstType == EntityType::Player &&
             secondType == EntityType::Enemy) {
    events_.publish(ShipContact{.player = first, .enemy = second});
  }
}

}  // namespace rtype::game
