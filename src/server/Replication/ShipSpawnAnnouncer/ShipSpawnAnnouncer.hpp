#pragma once

#include <cstdint>
#include <optional>
#include "ConnectedPlayer.hpp"
#include "EntitySpawnSender.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Gives a player who joins a ship in the world and tells the players
 * about it, as the Entities section of the protocol page describes.
 *
 * The ship appears at the start position of the player number (see
 * shipStartPosition). Every connected player, the new one included, is sent an
 * EntitySpawned for it, and the new player is sent one for each ship already
 * in the game, so it sees the others. A player who already has a ship gets no
 * second one and nobody is sent anything, so a call repeated for the same
 * player changes nothing.
 */
class ShipSpawnAnnouncer {
 public:
  /**
   * @brief What the announcer calls for each datagram to send.
   */
  using Send = SendDatagram;

  /**
   * @param world The world the ships are created in; it must outlive the
   * announcer.
   * @param roster The players of the game; it must outlive the announcer.
   * @param send Must not be empty.
   */
  ShipSpawnAnnouncer(game::World& world, PlayerRoster& roster, Send send);

  /**
   * @brief Creates the ship of @p player, unless it has one, and announces it.
   * @p player must be in the roster.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  void onPlayerJoined(ConnectedPlayer& player);

 private:
  /** @brief Sends @p recipient the EntitySpawned of the ship of @p owner. */
  void announce(ConnectedPlayer& recipient, const ConnectedPlayer& owner);

  /** @returns Where the entity named @p networkId is, nothing when no entity
   * has that id. */
  [[nodiscard]] std::optional<game::Position> positionOf(
      std::uint32_t networkId) const;

  game::World* world_;
  PlayerRoster* roster_;
  Send send_;
};

}  // namespace rtype::server
