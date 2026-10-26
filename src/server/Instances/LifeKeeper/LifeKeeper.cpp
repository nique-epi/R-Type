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
#include "NetworkIdentity.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "PlayerRoster.hpp"
#include "ShipContact.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @brief A contact that costs a life, read before anything is destroyed. */
struct CostlyContact {
  ConnectedPlayer* player{nullptr};
  std::uint32_t shipId{0};
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
 * @returns What @p contact costs, when it costs a life: the ship and the enemy
 * are in @p world, and the ship is the one of a player of @p roster who has a
 * life and is not invulnerable. Nothing otherwise.
 */
std::optional<CostlyContact> costlyContactOf(game::World& world,
                                             PlayerRoster& roster,
                                             const game::ShipContact& contact) {
  engine::ComponentRegistry& components = world.components();
  const game::NetworkIdentity* ship =
      components.get<game::NetworkIdentity>(contact.player);
  const game::NetworkIdentity* enemy =
      components.get<game::NetworkIdentity>(contact.enemy);
  if (ship == nullptr || enemy == nullptr) {
    return std::nullopt;
  }
  ConnectedPlayer* const player = ownerOf(roster, ship->networkId);
  if (player == nullptr || !canBeHit(*player)) {
    return std::nullopt;
  }
  return CostlyContact{.player = player,
                       .shipId = ship->networkId,
                       .enemy = contact.enemy,
                       .enemyId = enemy->networkId};
}

/** @brief Destroys the ship and the enemy of @p contact in @p world. */
void destroyBoth(game::World& world, const CostlyContact& contact) {
  world.removePlayer(contact.shipId);
  world.components().destroy(contact.enemy);
}

/** @brief Takes a life and the ship from @p player, and makes a player with a
 * life left wait RESPAWN_DELAY_TICKS ticks for a new ship. */
void takeLifeFrom(ConnectedPlayer& player) {
  --player.lives;
  player.shipEntityId = identifiers::NO_ENTITY_ID;
  player.ticksUntilRespawn = player.lives > 0 ? RESPAWN_DELAY_TICKS : 0;
}

/** @brief Publishes on @p events what @p contact cost, and logs it on
 * @p logger. */
void announce(engine::EventBus& events, const logging::Logger& logger,
              const CostlyContact& contact) {
  const ConnectedPlayer& player = *contact.player;
  events.publish(game::EntityDestruction{
      .networkId = contact.shipId, .cause = game::DestructionCause::Killed});
  events.publish(game::EntityDestruction{
      .networkId = contact.enemyId, .cause = game::DestructionCause::Killed});
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
      subscription_(events_.subscribe<game::ShipContact>(
          [this](const game::ShipContact& contact) {
            onShipContact(contact);
          })),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {}

LifeKeeper::~LifeKeeper() { events_.unsubscribe(subscription_); }

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
  takeLifeFrom(*costly->player);
  announce(events_, logger_, *costly);
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
