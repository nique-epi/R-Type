#include "PlayfieldExitSystem.hpp"
#include "Boss.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Direction.hpp"
#include "DirectionConstants.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "Intent.hpp"
#include "NetworkIdentity.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "SpawningConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @brief What decides whether a missile is out: its box, and where it
 * flies. */
struct MissileExit {
  CollisionBox box{};
  Direction heading{};
};

/** @returns Where a missile of @p type steered by @p intent flies: its
 * direction once a pilot has steered it somewhere, otherwise the way the
 * missiles of its side fly, right for a player, left for an enemy. */
Direction headingOf(const Intent* intent, EntityType type) {
  const bool steered =
      intent != nullptr && intent->steered &&
      (intent->direction.x != 0.0F || intent->direction.y != 0.0F);
  if (steered) {
    return intent->direction;
  }
  return Direction{
      .x = type == EntityType::PlayerMissile ? FULL_STEERING : -FULL_STEERING,
      .y = 0.0F};
}

/** @returns Whether a missile at @p position, as @p exit says, is wholly
 * beyond an edge it flies towards: more than half its box beyond it. */
bool hasFlownOut(const Position& position, const MissileExit& exit) {
  const float halfWidth = exit.box.width * engine::HALF;
  const float halfHeight = exit.box.height * engine::HALF;
  const Direction heading = exit.heading;
  return (heading.x < 0.0F && position.x < -halfWidth) ||
         (heading.x > 0.0F && position.x > PLAYFIELD_WIDTH + halfWidth) ||
         (heading.y < 0.0F && position.y < -halfHeight) ||
         (heading.y > 0.0F && position.y > PLAYFIELD_HEIGHT + halfHeight);
}

/**
 * @returns Whether an entity of @p type at @p position went out of the
 * playfield for good: an enemy more than ENEMY_EXIT_MARGIN beyond the left
 * edge, a missile wholly beyond an edge it flies towards (see hasFlownOut()),
 * with the box and the heading @p exit gives it. A ship never is.
 */
bool hasLeftThePlayfield(EntityType type, const Position& position,
                         const MissileExit& exit) {
  switch (type) {
    case EntityType::Enemy:
      return position.x < -ENEMY_EXIT_MARGIN;
    case EntityType::PlayerMissile:
    case EntityType::EnemyMissile:
      return hasFlownOut(position, exit);
    case EntityType::Player:
      return false;
  }
  return false;
}

/**
 * @returns The box and the heading of @p entity when @p type is a missile,
 * the only type whose exit hasLeftThePlayfield() measures with them: an empty
 * box without a CollisionBox. Nothing is looked up for any other type.
 */
MissileExit missileExitOf(engine::ComponentRegistry& components,
                          engine::Entity entity, EntityType type) {
  if (type != EntityType::PlayerMissile && type != EntityType::EnemyMissile) {
    return MissileExit{};
  }
  const CollisionBox* const box = components.get<CollisionBox>(entity);
  return MissileExit{
      .box = box == nullptr ? CollisionBox{} : *box,
      .heading = headingOf(components.get<Intent>(entity), type)};
}

}  // namespace

PlayfieldExitSystem::PlayfieldExitSystem(engine::EventBus& events)
    : events_(events) {}

void PlayfieldExitSystem::update(engine::ComponentRegistry& components,
                                 [[maybe_unused]] engine::Duration elapsed) {
  components.forEach<Position, EntityType>(
      [this, &components](engine::Entity entity, const Position& position,
                          const EntityType& type) {
        if (!hasLeftThePlayfield(type, position,
                                 missileExitOf(components, entity, type)) ||
            components.get<Boss>(entity) != nullptr) {
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
