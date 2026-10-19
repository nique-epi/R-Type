#include "ShipSpawnAnnouncer.hpp"
#include <cstdint>
#include <optional>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ShipStartPosition.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server {

ShipSpawnAnnouncer::ShipSpawnAnnouncer(game::World& world, PlayerRoster& roster,
                                       Send send)
    : world_(&world), roster_(&roster), send_(std::move(send)) {}

void ShipSpawnAnnouncer::onPlayerJoined(ConnectedPlayer& player) {
  if (player.shipEntityId != identifiers::NO_ENTITY_ID) {
    return;
  }
  player.shipEntityId = world_->spawnPlayer(
      game::shipStartPosition(player.playerNumber, network::MAXIMUM_PLAYERS));
  for (ConnectedPlayer* const other : roster_->connected()) {
    announce(*other, player);
    if (other != &player && other->shipEntityId != identifiers::NO_ENTITY_ID) {
      announce(player, *other);
    }
  }
}

void ShipSpawnAnnouncer::announce(ConnectedPlayer& recipient,
                                  const ConnectedPlayer& owner) {
  const std::optional<game::Position> position = positionOf(owner.shipEntityId);
  if (!position.has_value()) {
    return;
  }
  sendEntitySpawned(
      send_, recipient,
      network::EntitySpawned{.entityId = owner.shipEntityId,
                             .entityType = network::WireEntityType::PlayerShip,
                             .x = position->x,
                             .y = position->y});
}

std::optional<game::Position> ShipSpawnAnnouncer::positionOf(
    std::uint32_t networkId) const {
  std::optional<game::Position> found;
  world_->components().forEach<game::NetworkIdentity, game::Position>(
      [&found, networkId](engine::Entity /*entity*/,
                          const game::NetworkIdentity& identity,
                          const game::Position& position) {
        if (identity.networkId == networkId) {
          found = position;
        }
      });
  return found;
}

}  // namespace rtype::server
