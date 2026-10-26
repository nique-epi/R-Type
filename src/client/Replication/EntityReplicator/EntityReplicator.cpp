#include "EntityReplicator.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
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
#include "GameEvent.hpp"
#include "NetworkEntityTable.hpp"
#include "PilotShip.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
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

/**
 * @returns The game type of the entity a message announces, or nothing when
 * the client must not create it: the id NO_ENTITY_ID, or a type outside
 * WireEntityType.
 */
std::optional<game::EntityType> creatableTypeOf(
    const network::EntityState& announced) {
  if (announced.entityId == identifiers::NO_ENTITY_ID) {
    return std::nullopt;
  }
  return entityTypeOf(announced.entityType);
}

/** @returns The entity an EntitySpawned announces, as a WorldState lists
 * it. */
network::EntityState announcedBy(const network::EntitySpawned& message) {
  return network::EntityState{.entityId = message.entityId,
                              .entityType = message.entityType,
                              .x = message.x,
                              .y = message.y};
}

/** @returns The pilot color of a player number, the number less
 * FIRST_PLAYER_NUMBER, or nothing for a number outside FIRST_PLAYER_NUMBER to
 * MAXIMUM_PLAYERS. */
std::optional<int> pilotColorOf(std::uint8_t playerNumber) {
  if (playerNumber < network::FIRST_PLAYER_NUMBER ||
      playerNumber - network::FIRST_PLAYER_NUMBER >=
          static_cast<int>(network::MAXIMUM_PLAYERS)) {
    return std::nullopt;
  }
  return playerNumber - network::FIRST_PLAYER_NUMBER;
}

/** @brief Puts the entity at the position; does nothing for an entity
 * without a Position. */
void moveEntity(engine::ComponentRegistry& components, engine::Entity entity,
                game::Position position) {
  auto* current = components.get<game::Position>(entity);
  if (current != nullptr) {
    *current = position;
  }
}

/** @brief Plays the appearance of an entity created: the arrival of a ship,
 * the muzzle flash of a missile, nothing for a Bydo. */
void announce(EffectSpawner& effects, engine::ComponentRegistry& components,
              engine::Entity entity) {
  const game::EntityType* type = components.get<game::EntityType>(entity);
  const game::Position* position = components.get<game::Position>(entity);
  if (type == nullptr || position == nullptr) {
    return;
  }
  if (*type == game::EntityType::Player) {
    effects.playArrival(entity);
    return;
  }
  effects.playAppearance(*type, *position);
}

/** @brief Plays the explosion of the entity, centered where it stands: T4 for
 * a ship; does nothing for an entity without a Position or an EntityType. */
void explode(EffectSpawner& effects, engine::ComponentRegistry& components,
             engine::Entity entity) {
  const game::EntityType* type = components.get<game::EntityType>(entity);
  const game::Position* position = components.get<game::Position>(entity);
  if (type == nullptr || position == nullptr) {
    return;
  }
  if (*type == game::EntityType::Player) {
    effects.playShipDown(entity);
    return;
  }
  effects.playExplosion(*type, *position);
}

/** @returns The ship a network id names, or nothing when the client has no
 * entity of that id or it is no ship. */
std::optional<engine::Entity> shipOf(
    const engine::NetworkEntityTable& entityByNetworkId,
    engine::ComponentRegistry& components, std::uint32_t entityId) {
  const std::optional<engine::Entity> known = entityByNetworkId.find(entityId);
  if (!known.has_value()) {
    return std::nullopt;
  }
  const game::EntityType* type = components.get<game::EntityType>(*known);
  if (type == nullptr || *type != game::EntityType::Player) {
    return std::nullopt;
  }
  return known;
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
  if (const std::optional<engine::Entity> created =
          create(announcedBy(message));
      created.has_value()) {
    lastListedTicks_.appeared(message.entityId);
    announce(*effects_, *components_, *created);
  }
}

void EntityReplicator::apply(const network::WorldState& state) {
  if (!worldStateFilter_.accept(state.tick)) {
    return;
  }
  const std::vector<engine::Entity> created =
      applyEntities(state.entities, state.tick);
  removeUnlistedAt(state.tick);
  applyPlayers(state.players);
  for (const engine::Entity entity : created) {
    announce(*effects_, *components_, entity);
  }
}

void EntityReplicator::apply(const network::EntityDestroyed& message) {
  const std::optional<engine::Entity> known =
      entityByNetworkId_.find(message.entityId);
  if (!known.has_value()) {
    return;
  }
  if (message.reason == network::DestructionReason::Killed) {
    explode(*effects_, *components_, *known);
  }
  components_->destroy(*known);
  entityByNetworkId_.unbind(message.entityId);
  lastListedTicks_.forget(message.entityId);
}

void EntityReplicator::apply(const network::GameEvent& event) {
  if (event.kind != network::GameEventKind::EntityDamaged) {
    return;
  }
  if (const std::optional<engine::Entity> known =
          entityByNetworkId_.find(event.entityId);
      known.has_value()) {
    effects_->playHit(*known);
  }
}

std::optional<engine::Entity> EntityReplicator::create(
    const network::EntityState& announced) {
  const std::optional<game::EntityType> type = creatableTypeOf(announced);
  if (!type.has_value()) {
    return std::nullopt;
  }
  std::optional<AnimatedSprite> look =
      animatedSpriteOf(*type, announced.entityId);
  if (!look.has_value()) {
    return std::nullopt;
  }
  const engine::Entity entity =
      addEntity(*type, game::Position{.x = announced.x, .y = announced.y},
                std::move(*look));
  entityByNetworkId_.bind(announced.entityId, entity);
  return entity;
}

engine::Entity EntityReplicator::addEntity(game::EntityType type,
                                           game::Position position,
                                           AnimatedSprite look) {
  const engine::Entity entity = entities_->create();
  components_->add(entity, position);
  components_->add(entity, type);
  components_->add(entity, std::move(look.sprite));
  components_->add(entity, look.animation);
  return entity;
}

std::vector<engine::Entity> EntityReplicator::applyEntities(
    std::span<const network::EntityState> entities, std::uint32_t tick) {
  std::vector<engine::Entity> created;
  for (const network::EntityState& listed : entities) {
    const std::optional<engine::Entity> known =
        entityByNetworkId_.find(listed.entityId);
    if (known.has_value()) {
      moveEntity(*components_, *known,
                 game::Position{.x = listed.x, .y = listed.y});
      lastListedTicks_.listed(listed.entityId, tick);
    } else if (const std::optional<engine::Entity> entity = create(listed);
               entity.has_value()) {
      created.push_back(*entity);
      lastListedTicks_.listed(listed.entityId, tick);
    }
  }
  return created;
}

void EntityReplicator::applyPlayers(
    std::span<const network::PlayerState> players) {
  for (const network::PlayerState& player : players) {
    const std::optional<int> pilotColor = pilotColorOf(player.playerNumber);
    if (pilotColor.has_value()) {
      markShip(player, *pilotColor);
      effects_->noteScore(*pilotColor, player.score);
    }
  }
}

void EntityReplicator::markShip(const network::PlayerState& player,
                                int pilotColor) {
  const std::optional<engine::Entity> ship =
      shipOf(entityByNetworkId_, *components_, player.shipEntityId);
  if (!ship.has_value()) {
    return;
  }
  components_->add(*ship, PilotShip{.color = pilotColor});
  effects_->setInvulnerable(*ship, player.invulnerable);
}

void EntityReplicator::removeUnlistedAt(std::uint32_t tick) {
  for (const std::uint32_t networkId : lastListedTicks_.takeUnlistedAt(tick)) {
    const std::optional<engine::Entity> known =
        entityByNetworkId_.find(networkId);
    if (known.has_value()) {
      components_->destroy(*known);
      entityByNetworkId_.unbind(networkId);
    }
  }
}

}  // namespace rtype::client
