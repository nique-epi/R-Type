#include "EntityReplicator.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <utility>
#include <vector>
#include "AnimatedSprite.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityDestroyed.hpp"
#include "EntityFeedback.hpp"
#include "EntityIdConstants.hpp"
#include "EntityRegistry.hpp"
#include "EntitySpawned.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "GameEvent.hpp"
#include "ISoundPlayer.hpp"
#include "NetworkEntityTable.hpp"
#include "PilotShip.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SoundConstants.hpp"
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

/** @returns The sound of the appearance of an entity of that type: the shot
 * of a missile of a player or of an enemy; nothing for a ship or a Bydo. */
std::optional<std::string_view> appearanceSoundOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::PlayerMissile:
      return PLAYER_SHOT_SOUND_ID;
    case game::EntityType::EnemyMissile:
      return ENEMY_SHOT_SOUND_ID;
    case game::EntityType::Player:
    case game::EntityType::Enemy:
      return std::nullopt;
  }
  return std::nullopt;
}

/** @returns The sound of an entity of that type killed: the big explosion of
 * a ship, the small one of a Bydo; nothing for a missile, which only shows
 * its impact. */
std::optional<std::string_view> explosionSoundOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return BIG_EXPLOSION_SOUND_ID;
    case game::EntityType::Enemy:
      return SMALL_EXPLOSION_SOUND_ID;
    case game::EntityType::PlayerMissile:
    case game::EntityType::EnemyMissile:
      return std::nullopt;
  }
  return std::nullopt;
}

/** @brief Plays the sound, when there is one. */
void playSound(ISoundPlayer& sounds, std::optional<std::string_view> soundId) {
  if (soundId.has_value()) {
    sounds.play(*soundId);
  }
}

/** @brief Plays the appearance of an entity created: the arrival of a ship,
 * the muzzle flash and the shot of a missile, nothing for a Bydo. */
void announce(const EntityFeedback& feedback,
              engine::ComponentRegistry& components, engine::Entity entity) {
  const game::EntityType* type = components.get<game::EntityType>(entity);
  const game::Position* position = components.get<game::Position>(entity);
  if (type == nullptr || position == nullptr) {
    return;
  }
  playSound(feedback.sounds, appearanceSoundOf(*type));
  if (*type == game::EntityType::Player) {
    feedback.effects.get().playArrival(entity);
    return;
  }
  feedback.effects.get().playAppearance(*type, *position);
}

/** @brief Plays the explosion of the entity, centered where it stands, and
 * its sound: T4 for a ship; does nothing for an entity without a Position or
 * an EntityType. */
void explode(const EntityFeedback& feedback,
             engine::ComponentRegistry& components, engine::Entity entity) {
  const game::EntityType* type = components.get<game::EntityType>(entity);
  const game::Position* position = components.get<game::Position>(entity);
  if (type == nullptr || position == nullptr) {
    return;
  }
  playSound(feedback.sounds, explosionSoundOf(*type));
  if (*type == game::EntityType::Player) {
    feedback.effects.get().playShipDown(entity);
    return;
  }
  feedback.effects.get().playExplosion(*type, *position);
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
                                   EntityFeedback feedback)
    : entities_(&entities), components_(&components), feedback_(feedback) {}

void EntityReplicator::apply(const network::EntitySpawned& message) {
  if (entityByNetworkId_.find(message.entityId).has_value()) {
    return;
  }
  if (const std::optional<engine::Entity> created =
          create(announcedBy(message));
      created.has_value()) {
    announce(feedback_, *components_, *created);
  }
}

void EntityReplicator::apply(const network::WorldState& state) {
  if (!worldStateFilter_.accept(state.tick)) {
    return;
  }
  const std::vector<engine::Entity> created = applyEntities(state.entities);
  applyPlayers(state.players);
  for (const engine::Entity entity : created) {
    announce(feedback_, *components_, entity);
  }
}

void EntityReplicator::apply(const network::EntityDestroyed& message) {
  const std::optional<engine::Entity> known =
      entityByNetworkId_.find(message.entityId);
  if (!known.has_value()) {
    return;
  }
  if (message.reason == network::DestructionReason::Killed) {
    explode(feedback_, *components_, *known);
  }
  components_->destroy(*known);
  entityByNetworkId_.unbind(message.entityId);
}

void EntityReplicator::apply(const network::GameEvent& event) {
  if (event.kind == network::GameEventKind::PlayerHit) {
    feedback_.sounds.get().play(PLAYER_HIT_SOUND_ID);
    return;
  }
  if (event.kind != network::GameEventKind::EntityDamaged) {
    return;
  }
  if (const std::optional<engine::Entity> known =
          entityByNetworkId_.find(event.entityId);
      known.has_value()) {
    feedback_.effects.get().playHit(*known);
    feedback_.sounds.get().play(HIT_SOUND_ID);
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
    std::span<const network::EntityState> entities) {
  std::vector<engine::Entity> created;
  for (const network::EntityState& listed : entities) {
    const std::optional<engine::Entity> known =
        entityByNetworkId_.find(listed.entityId);
    if (known.has_value()) {
      moveEntity(*components_, *known,
                 game::Position{.x = listed.x, .y = listed.y});
    } else if (const std::optional<engine::Entity> entity = create(listed);
               entity.has_value()) {
      created.push_back(*entity);
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
      feedback_.effects.get().noteScore(*pilotColor, player.score);
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
  feedback_.effects.get().setInvulnerable(*ship, player.invulnerable);
}

}  // namespace rtype::client
