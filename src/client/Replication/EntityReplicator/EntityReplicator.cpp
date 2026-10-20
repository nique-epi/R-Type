#include "EntityReplicator.hpp"
#include <optional>
#include <utility>
#include "AnimatedSprite.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityDestroyed.hpp"
#include "EntityIdConstants.hpp"
#include "EntityRegistry.hpp"
#include "EntitySpawned.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "Position.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/**
 * @returns The game type of an entity the protocol names @p wireType, as the
 * entity type table of the protocol page lists them, or nothing for a value
 * outside WireEntityType. It is the inverse of what the server sends: a type
 * added to the protocol needs its case here and on the server.
 */
std::optional<game::EntityType> entityTypeOf(network::WireEntityType wireType) {
  switch (wireType) {
    case network::WireEntityType::PlayerShip:
      return game::EntityType::Player;
    case network::WireEntityType::PlayerMissile:
      return game::EntityType::PlayerMissile;
    case network::WireEntityType::Bydo:
      return game::EntityType::Enemy;
    case network::WireEntityType::EnemyMissile:
      return game::EntityType::EnemyMissile;
  }
  return std::nullopt;
}

}  // namespace

EntityReplicator::EntityReplicator(engine::EntityRegistry& entities,
                                   engine::ComponentRegistry& components,
                                   EffectSpawner& effects)
    : entities_(&entities), components_(&components), effects_(&effects) {}

void EntityReplicator::apply(const network::EntitySpawned& message) {
  if (entityByNetworkId_.find(message.entityId).has_value()) {
    return;
  }
  create(network::EntityState{.entityId = message.entityId,
                              .entityType = message.entityType,
                              .x = message.x,
                              .y = message.y});
}

void EntityReplicator::apply(const network::WorldState& state) {
  if (!worldStateFilter_.accept(state.tick)) {
    return;
  }
  for (const network::EntityState& listed : state.entities) {
    const std::optional<engine::Entity> known =
        entityByNetworkId_.find(listed.entityId);
    if (known.has_value()) {
      move(*known, game::Position{.x = listed.x, .y = listed.y});
    } else {
      create(listed);
    }
  }
}

void EntityReplicator::apply(const network::EntityDestroyed& message) {
  const std::optional<engine::Entity> known =
      entityByNetworkId_.find(message.entityId);
  if (!known.has_value()) {
    return;
  }
  if (message.reason == network::DestructionReason::Killed) {
    explode(*known);
  }
  components_->destroy(*known);
  entityByNetworkId_.unbind(message.entityId);
}

void EntityReplicator::create(const network::EntityState& announced) {
  if (announced.entityId == identifiers::NO_ENTITY_ID) {
    return;
  }
  const std::optional<game::EntityType> type =
      entityTypeOf(announced.entityType);
  if (!type.has_value()) {
    return;
  }
  std::optional<AnimatedSprite> look =
      animatedSpriteOf(*type, announced.entityId);
  if (!look.has_value()) {
    return;
  }
  const game::Position position{.x = announced.x, .y = announced.y};
  const engine::Entity entity = entities_->create();
  components_->add(entity, position);
  components_->add(entity, *type);
  components_->add(entity, std::move(look->sprite));
  components_->add(entity, look->animation);
  entityByNetworkId_.bind(announced.entityId, entity);
  effects_->playAppearance(*type, position);
}

void EntityReplicator::move(engine::Entity entity, game::Position position) {
  auto* current = components_->get<game::Position>(entity);
  if (current != nullptr) {
    *current = position;
  }
}

void EntityReplicator::explode(engine::Entity entity) {
  const game::EntityType* type = components_->get<game::EntityType>(entity);
  const game::Position* position = components_->get<game::Position>(entity);
  if (type != nullptr && position != nullptr) {
    effects_->playExplosion(*type, *position);
  }
}

}  // namespace rtype::client
