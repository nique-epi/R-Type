#pragma once

#include <functional>
#include <optional>
#include "ConnectedPlayer.hpp"
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "PlayerRoster.hpp"
#include "ShipSpawnAnnouncerParts.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Gives a player who joins a ship in the world and tells the players
 * about it, as the Entities section of the protocol page describes.
 *
 * The ship is made from the sheet of a ship (see game::World::spawn), at the
 * start position of the player number (see shipStartPosition), with the lives
 * the player has left. Every connected player, the new one included, is sent an
 * EntitySpawned for it, and the new player is sent one for each ship already
 * in the game, so it sees the others. A player who already has a ship gets no
 * second one and nobody is sent anything, so a call repeated for the same
 * player changes nothing.
 */
class ShipSpawnAnnouncer {
 public:
  /** @param parts The world, the players, how to send, and the sheet of a
   * ship. */
  explicit ShipSpawnAnnouncer(ShipSpawnAnnouncerParts parts);

  /**
   * @brief Creates the ship of @p player, unless it has one, and announces it.
   * @p player must be in the roster.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  void onPlayerJoined(ConnectedPlayer& player);

 private:
  /** @brief Makes the ship of @p player from the sheet of a ship at the start
   * of its player number, with the lives the player has left. */
  void giveShipTo(ConnectedPlayer& player);

  /**
   * @returns The EntitySpawned of the ship of @p owner, at the position it has
   * in the world, or nothing when @p owner has no ship there.
   */
  [[nodiscard]] std::optional<network::EntitySpawned> shipSpawnOf(
      const ConnectedPlayer& owner) const;

  /** @brief Sends @p recipient the EntitySpawned of the ship of @p owner, or
   * nothing when @p owner has no ship in the world. */
  void announce(ConnectedPlayer& recipient, const ConnectedPlayer& owner);

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  std::reference_wrapper<const game::EntitySheet> ship_;
};

}  // namespace rtype::server
