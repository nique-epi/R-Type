#include "LifeKeeper.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityIdConstants.hpp"
#include "EventBus.hpp"
#include "GameInstanceConstants.hpp"
#include "LifeKeeperConstants.hpp"
#include "LifeKeeperParts.hpp"
#include "Logger.hpp"
#include "MissileHit.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "PlayerRoster.hpp"
#include "ShipContact.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @brief The ship of a player that a contact or a hit costs a life, read
 * before anything is destroyed. */
struct DoomedShip {
  ConnectedPlayer* player{nullptr};
  std::uint32_t shipId{0};
};

/** @brief A contact that costs a life, read before anything is destroyed. */
struct CostlyContact {
  DoomedShip ship;
  engine::Entity enemy;
  std::uint32_t enemyId{0};
};

/** @returns The connected player of @p roster whose ship is @p shipId, or
 * nullptr when none has it. */
ConnectedPlayer* ownerOf(PlayerRoster& roster, std::uint32_t shipId) {
  for (ConnectedPlayer* const player : roster.connected()) {
    if (player->shipEntityId == shipId) {
      return player;
    }
  }
  return nullptr;
}

/** @returns Whether the ship of @p player can be hit: the player has a life
 * left and is not invulnerable. */
bool canBeHit(const ConnectedPlayer& player) {
  return player.lives > 0 && player.invulnerableTicksLeft == 0;
}

/**
 * @returns The entity @p ship of @p world as a DoomedShip, when it is the ship
 * of a player of @p roster who has a life and is not invulnerable. Nothing
 * otherwise: an entity already gone, a Bydo, a ship out of reach.
 */
std::optional<DoomedShip> doomedShipOf(game::World& world, PlayerRoster& roster,
                                       engine::Entity ship) {
  const game::NetworkIdentity* identity =
      world.components().get<game::NetworkIdentity>(ship);
  if (identity == nullptr) {
    return std::nullopt;
  }
  ConnectedPlayer* const player = ownerOf(roster, identity->networkId);
  if (player == nullptr || !canBeHit(*player)) {
    return std::nullopt;
  }
  return DoomedShip{.player = player, .shipId = identity->networkId};
}

/**
 * @returns What @p contact costs, when it costs a life: the enemy is in @p
 * world, and the ship is one doomedShipOf() gives. Nothing otherwise.
 */
std::optional<CostlyContact> costlyContactOf(game::World& world,
                                             PlayerRoster& roster,
                                             const game::ShipContact& contact) {
  const game::NetworkIdentity* enemy =
      world.components().get<game::NetworkIdentity>(contact.enemy);
  if (enemy == nullptr) {
    return std::nullopt;
  }
  const std::optional<DoomedShip> ship =
      doomedShipOf(world, roster, contact.player);
  if (!ship.has_value()) {
    return std::nullopt;
  }
  return CostlyContact{
      .ship = *ship, .enemy = contact.enemy, .enemyId = enemy->networkId};
}

/** @brief Destroys the ship and the enemy of @p contact in @p world. */
void destroyBoth(game::World& world, const CostlyContact& contact) {
  world.removePlayer(contact.ship.shipId);
  world.components().destroy(contact.enemy);
}

/** @brief Takes a life and the ship from @p player, and makes a player with a
 * life left wait RESPAWN_DELAY_TICKS ticks for a new ship. */
void takeLifeFrom(ConnectedPlayer& player) {
  --player.lives;
  player.shipEntityId = identifiers::NO_ENTITY_ID;
  player.ticksUntilRespawn = player.lives > 0 ? RESPAWN_DELAY_TICKS : 0;
}

/** @brief Publishes on @p events the EntityDestruction (Killed) of the entity
 * @p networkId. */
void announceKill(engine::EventBus& events, std::uint32_t networkId) {
  events.publish(game::EntityDestruction{
      .networkId = networkId, .cause = game::DestructionCause::Killed});
}

/** @brief Publishes on @p events the PlayerHit of @p player, then its
 * PlayerElimination when it has no life left, and logs it on @p logger. */
void announceLifeLost(engine::EventBus& events, const logging::Logger& logger,
                      const ConnectedPlayer& player) {
  events.publish(game::PlayerHit{.playerNumber = player.playerNumber});
  const auto playerNumber = static_cast<unsigned int>(player.playerNumber);
  if (player.lives > 0) {
    logger.info("player ", playerNumber, " lost a life, ",
                static_cast<unsigned int>(player.lives), " left");
    return;
  }
  events.publish(game::PlayerElimination{.playerNumber = player.playerNumber});
  logger.info("player ", playerNumber, " lost its last life and now watches");
}

/** @brief Counts one tick of invulnerability of @p player down, when it has
 * some left. */
void countInvulnerabilityDown(ConnectedPlayer& player) {
  if (player.invulnerableTicksLeft > 0) {
    --player.invulnerableTicksLeft;
  }
}

}  // namespace

LifeKeeper::LifeKeeper(LifeKeeperParts parts)
    : events_(parts.events),
      world_(parts.world),
      roster_(parts.roster),
      respawn_(std::move(parts.respawn)),
      contactSubscription_(events_.subscribe<game::ShipContact>(
          [this](const game::ShipContact& contact) {
            onShipContact(contact);
          })),
      hitSubscription_(events_.subscribe<game::MissileHit>(
          [this](const game::MissileHit& hit) { onMissileHit(hit); })),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {}

LifeKeeper::~LifeKeeper() {
  events_.unsubscribe(hitSubscription_);
  events_.unsubscribe(contactSubscription_);
}

void LifeKeeper::onTick() {
  for (ConnectedPlayer* const player : roster_.connected()) {
    countInvulnerabilityDown(*player);
    countWaitDown(*player);
  }
}

void LifeKeeper::onShipContact(const game::ShipContact& contact) {
  const std::optional<CostlyContact> costly =
      costlyContactOf(world_, roster_, contact);
  if (!costly.has_value()) {
    return;
  }
  destroyBoth(world_, *costly);
  takeLifeFrom(*costly->ship.player);
  announceKill(events_, costly->ship.shipId);
  announceKill(events_, costly->enemyId);
  announceLifeLost(events_, logger_, *costly->ship.player);
}

void LifeKeeper::onMissileHit(const game::MissileHit& hit) {
  const std::optional<DoomedShip> ship =
      doomedShipOf(world_, roster_, hit.target);
  if (!ship.has_value()) {
    return;
  }
  world_.removePlayer(ship->shipId);
  takeLifeFrom(*ship->player);
  announceKill(events_, ship->shipId);
  announceLifeLost(events_, logger_, *ship->player);
}

void LifeKeeper::countWaitDown(ConnectedPlayer& player) {
  if (player.ticksUntilRespawn == 0) {
    return;
  }
  --player.ticksUntilRespawn;
  if (player.ticksUntilRespawn == 0) {
    respawn(player);
  }
}

void LifeKeeper::respawn(ConnectedPlayer& player) {
  try {
    respawn_(player);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("player ", static_cast<unsigned int>(player.playerNumber),
                 " gets no new ship: every network id was given");
    return;
  }
  player.invulnerableTicksLeft = INVULNERABILITY_TICKS;
}

}  // namespace rtype::server
