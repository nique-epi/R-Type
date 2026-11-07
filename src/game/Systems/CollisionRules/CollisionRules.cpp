#include "CollisionRules.hpp"
#include "Collision.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "MissileHit.hpp"
#include "ShipContact.hpp"
#include "TypedEntity.hpp"
#include "ViewDelay.hpp"

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
  const TypedEntity first{.entity = collision.first, .type = *firstType};
  const TypedEntity second{.entity = collision.second, .type = *secondType};
  publishIfRuleMatches(first, second);
  publishIfRuleMatches(second, first);
}

void CollisionRules::publishIfRuleMatches(TypedEntity striker,
                                          TypedEntity struck) {
  const bool playerMissileHitsEnemy =
      striker.type == EntityType::PlayerMissile &&
      struck.type == EntityType::Enemy &&
      !components_.has<ViewDelay>(striker.entity);
  const bool enemyMissileHitsPlayer =
      striker.type == EntityType::EnemyMissile &&
      struck.type == EntityType::Player &&
      !components_.has<ViewDelay>(struck.entity);
  if (playerMissileHitsEnemy || enemyMissileHitsPlayer) {
    events_.publish(
        MissileHit{.missile = striker.entity, .target = struck.entity});
  } else if (striker.type == EntityType::Player &&
             struck.type == EntityType::Enemy &&
             !components_.has<ViewDelay>(striker.entity)) {
    events_.publish(
        ShipContact{.player = striker.entity, .enemy = struck.entity});
  }
}

}  // namespace rtype::game
