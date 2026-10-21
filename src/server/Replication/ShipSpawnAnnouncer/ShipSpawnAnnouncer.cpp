#include "ShipSpawnAnnouncer.hpp"
#include <optional>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
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

std::optional<network::EntitySpawned> ShipSpawnAnnouncer::shipSpawnOf(
    const ConnectedPlayer& owner) const {
  const game::Position* const position =
      world_->shipPositionOf(owner.shipEntityId);
  if (position == nullptr) {
    return std::nullopt;
  }
  return network::EntitySpawned{
      .entityId = owner.shipEntityId,
      .entityType = network::WireEntityType::PlayerShip,
      .x = position->x,
      .y = position->y};
}

void ShipSpawnAnnouncer::announce(ConnectedPlayer& recipient,
                                  const ConnectedPlayer& owner) {
  const std::optional<network::EntitySpawned> spawn = shipSpawnOf(owner);
  if (spawn.has_value()) {
    sendEntitySpawned(send_, recipient, *spawn);
  }
}

}  // namespace rtype::server
