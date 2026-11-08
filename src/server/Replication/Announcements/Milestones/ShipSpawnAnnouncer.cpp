#include "ShipSpawnAnnouncer.hpp"
#include <optional>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumber.hpp"
#include "Lives.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ShipSpawnAnnouncerParts.hpp"
#include "ShipStartPosition.hpp"
#include "World.hpp"

namespace rtype::server {

ShipSpawnAnnouncer::ShipSpawnAnnouncer(ShipSpawnAnnouncerParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      ship_(parts.ship) {}

void ShipSpawnAnnouncer::onPlayerJoined(ConnectedPlayer& player) {
  if (player.shipEntityId != identifiers::NO_ENTITY_ID) {
    return;
  }
  giveShipTo(player);
  for (ConnectedPlayer* const other : roster_->connected()) {
    announce(*other, player);
    if (other != &player && other->shipEntityId != identifiers::NO_ENTITY_ID) {
      announce(player, *other);
    }
  }
}

void ShipSpawnAnnouncer::giveShipTo(ConnectedPlayer& player) {
  player.shipEntityId = world_->spawn(
      ship_.get(),
      game::shipStartPosition(player.playerNumber, network::MAXIMUM_PLAYERS),
      player.playerNumber);
  auto* const lives = world_->shipComponentOf<game::Lives>(player.shipEntityId);
  if (lives != nullptr) {
    lives->count = player.livesWithoutShip;
  }
}

std::optional<network::EntitySpawned> ShipSpawnAnnouncer::shipSpawnOf(
    const ConnectedPlayer& owner) const {
  const game::Position* const position =
      world_->shipPositionOf(owner.shipEntityId);
  const game::EntityTypeNumber* const typeNumber =
      world_->shipComponentOf<game::EntityTypeNumber>(owner.shipEntityId);
  if (position == nullptr || typeNumber == nullptr) {
    return std::nullopt;
  }
  return entitySpawnedOf(owner.shipEntityId, *typeNumber, *position);
}

void ShipSpawnAnnouncer::announce(ConnectedPlayer& recipient,
                                  const ConnectedPlayer& owner) {
  const std::optional<network::EntitySpawned> spawn = shipSpawnOf(owner);
  if (spawn.has_value()) {
    sendEntitySpawned(send_, recipient, *spawn);
  }
}

}  // namespace rtype::server
