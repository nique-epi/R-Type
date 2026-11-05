#include "PlayfieldExitSystem.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "NetworkIdentity.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "SpawningConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/**
 * @returns Whether an entity of @p type at @p position, half of whose box is
 * @p halfWidth wide, went out of the playfield for good: an enemy more than
 * ENEMY_EXIT_MARGIN beyond the left edge, a missile more than @p halfWidth
 * beyond the edge it flies to, the left one for a missile of an enemy and the
 * right one for a missile of a player, so the whole missile is off screen. A
 * ship never is.
 */
bool hasLeftThePlayfield(EntityType type, const Position& position,
                         float halfWidth) {
  switch (type) {
    case EntityType::Enemy:
      return position.x < -ENEMY_EXIT_MARGIN;
    case EntityType::PlayerMissile:
      return position.x > PLAYFIELD_WIDTH + halfWidth;
    case EntityType::EnemyMissile:
      return position.x < -halfWidth;
    case EntityType::Player:
      return false;
  }
  return false;
}

/**
 * @returns Half the width of the CollisionBox of @p entity, 0 without one, when
 * @p type is a missile, the only type whose exit hasLeftThePlayfield() measures
 * with it; 0 for any other type, whose box is not looked up.
 */
float missileHalfWidthOf(engine::ComponentRegistry& components,
                         engine::Entity entity, EntityType type) {
  if (type != EntityType::PlayerMissile && type != EntityType::EnemyMissile) {
    return 0.0F;
  }
  const CollisionBox* const box = components.get<CollisionBox>(entity);
  return box == nullptr ? 0.0F : box->width * engine::HALF;
}

}  // namespace

PlayfieldExitSystem::PlayfieldExitSystem(engine::EventBus& events)
    : events_(events) {}

void PlayfieldExitSystem::update(engine::ComponentRegistry& components,
                                 [[maybe_unused]] engine::Duration elapsed) {
  components.forEach<Position, EntityType>(
      [this, &components](engine::Entity entity, const Position& position,
                          const EntityType& type) {
        if (!hasLeftThePlayfield(
                type, position, missileHalfWidthOf(components, entity, type))) {
          return;
        }
        const NetworkIdentity* identity =
            components.get<NetworkIdentity>(entity);
        if (identity == nullptr) {
          return;
        }
        events_.publish(EntityDestruction{.networkId = identity->networkId,
                                          .cause = DestructionCause::Removed});
        components.destroy(entity);
      });
}

}  // namespace rtype::game
