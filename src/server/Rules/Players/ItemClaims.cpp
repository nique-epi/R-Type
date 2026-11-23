#include "ItemClaims.hpp"
#include <algorithm>
#include <optional>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "ItemClaim.hpp"
#include "ItemTouch.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @returns Whether the pilot of @p claim is still in @p roster and still
 * flies the ship that touched the item: a ship destroyed in the same tick
 * resets the ship of its player. */
bool isStillFlown(PlayerRoster& roster, const ItemClaim& claim) {
  const ConnectedPlayer* const player = roster.findByNumber(claim.playerNumber);
  return player != nullptr && player->shipEntityId == claim.shipId;
}

}  // namespace

std::optional<ItemClaim> itemClaimOf(game::World& world, PlayerRoster& roster,
                                     ItemTouch touch) {
  const auto* const item =
      world.components().get<game::NetworkIdentity>(touch.item);
  const auto* const ship =
      world.components().get<game::NetworkIdentity>(touch.ship);
  if (item == nullptr || ship == nullptr) {
    return std::nullopt;
  }
  const ConnectedPlayer* const player = roster.findByShip(ship->networkId);
  if (player == nullptr) {
    return std::nullopt;
  }
  return ItemClaim{.item = touch.item,
                   .itemId = item->networkId,
                   .playerNumber = player->playerNumber,
                   .shipId = ship->networkId};
}

std::vector<ItemClaim> winningClaims(std::vector<ItemClaim> claims,
                                     PlayerRoster& roster) {
  std::ranges::sort(claims, [](const ItemClaim& left, const ItemClaim& right) {
    return std::pair{left.itemId, left.playerNumber} <
           std::pair{right.itemId, right.playerNumber};
  });
  std::vector<ItemClaim> winners;
  for (const ItemClaim& claim : claims) {
    const bool itemWon =
        !winners.empty() && winners.back().itemId == claim.itemId;
    if (!itemWon && isStillFlown(roster, claim)) {
      winners.push_back(claim);
    }
  }
  return winners;
}

}  // namespace rtype::server
