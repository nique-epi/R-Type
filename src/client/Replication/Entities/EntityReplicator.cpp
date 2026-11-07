#include "EntityReplicator.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "AnimatedSprite.hpp"
#include "AppearanceReference.hpp"
#include "AwaitedArrival.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityCues.hpp"
#include "EntityDestroyed.hpp"
#include "EntityFeedback.hpp"
#include "EntityIdConstants.hpp"
#include "EntityRegistry.hpp"
#include "EntityReplicatorParts.hpp"
#include "EntitySpawned.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "EntityTypeLook.hpp"
#include "GameEvent.hpp"
#include "IEntityTypeSource.hpp"
#include "ISoundPlayer.hpp"
#include "NetworkEntityTable.hpp"
#include "NetworkIdentity.hpp"
#include "PilotShip.hpp"
#include "Position.hpp"
#include "PositionHistory.hpp"
#include "PositionSample.hpp"
#include "ProtocolConstants.hpp"
#include "ShipFlight.hpp"
#include "SoundConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/**
 * @returns The look of the entity a message announces, or nothing when the
 * client must not create it: the id NO_ENTITY_ID, or a type number @p types
 * does not know.
 */
std::optional<EntityTypeLook> creatableLookOf(
    const IEntityTypeSource& types, const network::EntityState& announced) {
  if (announced.entityId == identifiers::NO_ENTITY_ID) {
    return std::nullopt;
  }
  return types.lookOf(announced.entityType);
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

/** @returns Whether the motion of @p player holds the bit at @p position. */
bool holdsMotionBit(const network::PlayerState& player, std::uint8_t position) {
  const unsigned int mask = 1U << static_cast<unsigned int>(position);
  return (static_cast<unsigned int>(player.motion) & mask) != 0U;
}

/**
 * @returns The ShipFlight of @p ship in @p components, a new one the first
 * time, with what the motion of @p player says its pilot does: pushing
 * forward (right without left), moving up or down, rolling, dashing.
 */
ShipFlight flightOf(engine::ComponentRegistry& components, engine::Entity ship,
                    const network::PlayerState& player) {
  const ShipFlight* const known = components.get<ShipFlight>(ship);
  ShipFlight flight = known != nullptr ? *known : ShipFlight{};
  const bool upHeld = holdsMotionBit(player, network::UP_ACTION_BIT_POSITION);
  const bool downHeld =
      holdsMotionBit(player, network::DOWN_ACTION_BIT_POSITION);
  flight.forward = holdsMotionBit(player, network::RIGHT_ACTION_BIT_POSITION) &&
                   !holdsMotionBit(player, network::LEFT_ACTION_BIT_POSITION);
  flight.sideways = upHeld != downHeld;
  flight.rolling = holdsMotionBit(player, network::ROLLING_MOTION_BIT_POSITION);
  flight.dashing = holdsMotionBit(player, network::DASHING_MOTION_BIT_POSITION);
  return flight;
}

/** @brief Puts the entity at the position of the sample and remembers the
 * sample for interpolation; does nothing for what the entity lacks (a
 * Position, a PositionHistory). */
void moveEntity(engine::ComponentRegistry& components, engine::Entity entity,
                PositionSample sample) {
  if (auto* const current = components.get<game::Position>(entity)) {
    *current = sample.position;
  }
  if (auto* const history = components.get<PositionHistory>(entity)) {
    history->record(sample);
  }
}

/** @returns Whether an entity of @p type appears only once the tick the client
 * shows reaches its first state: a missile, which the server creates at the
 * muzzle of a shooter the client shows a little in the past. A ship and a
 * Bydo appear as soon as they are announced. */
bool awaitsTheShownTick(game::EntityType type) {
  return type == game::EntityType::PlayerMissile ||
         type == game::EntityType::EnemyMissile;
}

/** @brief Plays the appearance of @p entity now, unless it awaits the tick
 * shown: revealArrivedEntities() plays it then. */
void announceUnlessAwaited(const EntityFeedback& feedback,
                           engine::ComponentRegistry& components,
                           engine::Entity entity) {
  if (!components.has<AwaitedArrival>(entity)) {
    announce(feedback, components, entity);
  }
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

EntityReplicator::EntityReplicator(EntityReplicatorParts parts)
    : entities_(&parts.entities.get()),
      components_(&parts.components.get()),
      feedback_(parts.feedback),
      types_(&parts.types.get()) {}

void EntityReplicator::apply(const network::EntitySpawned& message) {
  if (entityByNetworkId_.find(message.entityId).has_value()) {
    return;
  }
  if (const std::optional<engine::Entity> created =
          create(announcedBy(message));
      created.has_value()) {
    lastListedTicks_.appeared(message.entityId);
    announceUnlessAwaited(feedback_, *components_, *created);
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
    announceUnlessAwaited(feedback_, *components_, entity);
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
  lastListedTicks_.forget(message.entityId);
}

void EntityReplicator::apply(const network::GameEvent& event) {
  ISoundPlayer& sounds = feedback_.sounds.get();
  if (event.kind == network::GameEventKind::PlayerHit) {
    sounds.play(PLAYER_HIT_SOUND_ID);
    return;
  }
  if (event.kind != network::GameEventKind::EntityDamaged) {
    return;
  }
  if (const std::optional<engine::Entity> known =
          entityByNetworkId_.find(event.entityId);
      known.has_value()) {
    feedback_.effects.get().playHit(*known);
    sounds.play(HIT_SOUND_ID);
  }
}

std::optional<engine::Entity> EntityReplicator::create(
    const network::EntityState& announced) {
  const std::optional<EntityTypeLook> look =
      creatableLookOf(*types_, announced);
  if (!look.has_value()) {
    return std::nullopt;
  }
  const engine::Entity entity = addEntity(
      look->side, game::Position{.x = announced.x, .y = announced.y},
      animatedSpriteOf(look->appearance.get(), look->side, announced.entityId));
  components_->add(entity, game::NetworkIdentity{announced.entityId});
  components_->add(entity, look->hitbox);
  components_->add(entity, AppearanceReference{&look->appearance.get()});
  entityByNetworkId_.bind(announced.entityId, entity);
  return entity;
}

engine::Entity EntityReplicator::addEntity(game::EntityType type,
                                           game::Position position,
                                           AnimatedSprite look) {
  const engine::Entity entity = entities_->create();
  components_->add(entity, position);
  components_->add(entity, PositionHistory{});
  components_->add(entity, type);
  if (awaitsTheShownTick(type)) {
    components_->add(entity, AwaitedArrival{.sprite = std::move(look.sprite)});
  } else {
    components_->add(entity, std::move(look.sprite));
  }
  components_->add(entity, look.animation);
  return entity;
}

std::vector<engine::Entity> EntityReplicator::applyEntities(
    std::span<const network::EntityState> entities, std::uint32_t tick) {
  std::vector<engine::Entity> created;
  for (const network::EntityState& listed : entities) {
    const PositionSample sample{.tick = tick,
                                .position = {.x = listed.x, .y = listed.y}};
    const std::optional<engine::Entity> known =
        entityByNetworkId_.find(listed.entityId);
    if (known.has_value()) {
      moveEntity(*components_, *known, sample);
      lastListedTicks_.listed(listed.entityId, tick);
    } else if (const std::optional<engine::Entity> entity = create(listed);
               entity.has_value()) {
      moveEntity(*components_, *entity, sample);
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
    if (!pilotColor.has_value()) {
      continue;
    }
    if (const std::optional<engine::Entity> ship =
            shipEntityOf(player.shipEntityId)) {
      components_->add(*ship, PilotShip{.color = *pilotColor});
      components_->add(*ship, flightOf(*components_, *ship, player));
      feedback_.effects.get().setInvulnerable(*ship, player.invulnerable);
    }
    feedback_.effects.get().noteScore(*pilotColor, player.score);
  }
}

std::optional<engine::Entity> EntityReplicator::shipEntityOf(
    std::uint32_t networkId) const {
  return shipOf(entityByNetworkId_, *components_, networkId);
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
