#include "PlayfieldExitSystem.hpp"
#include "CollisionConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntitySizeConstants.hpp"
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
 * @returns Whether an entity of @p type at @p position went out of the
 * playfield for good: an enemy more than BYDO_EXIT_MARGIN beyond the left edge,
 * a missile of a player more than half its width beyond the right edge, so the
 * whole missile is off screen. A ship and a missile of an enemy never are.
 */
bool hasLeftThePlayfield(EntityType type, const Position& position) {
  switch (type) {
    case EntityType::Enemy:
      return position.x < -BYDO_EXIT_MARGIN;
    case EntityType::PlayerMissile:
      return position.x >
             PLAYFIELD_WIDTH + (PLAYER_MISSILE_WIDTH * engine::HALF);
    case EntityType::Player:
    case EntityType::EnemyMissile:
      return false;
  }
  return false;
}

}  // namespace

PlayfieldExitSystem::PlayfieldExitSystem(engine::EventBus& events)
    : events_(events) {}

void PlayfieldExitSystem::update(engine::ComponentRegistry& components,
                                 engine::Duration /*elapsed*/) {
  components.forEach<Position, EntityType>(
      [this, &components](engine::Entity entity, const Position& position,
                          const EntityType& type) {
        if (!hasLeftThePlayfield(type, position)) {
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
