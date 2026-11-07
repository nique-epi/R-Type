#include "LookItemKeeper.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EncodedMessage.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "LookItem.hpp"
#include "LookItemTouched.hpp"
#include "LookItemWon.hpp"
#include "MessageDelivery.hpp"
#include "MessagePayload.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @returns The claim of the pilot of the ship of @p touched on the item,
 * when the item is in @p world and the ship belongs to a player of @p roster.
 * Nothing otherwise. */
std::optional<LookItemClaim> claimOf(game::World& world, PlayerRoster& roster,
                                     const game::LookItemTouched& touched) {
  const game::NetworkIdentity* const item =
      world.components().get<game::NetworkIdentity>(touched.item);
  const game::LookItem* const look =
      world.components().get<game::LookItem>(touched.item);
  const game::NetworkIdentity* const ship =
      world.components().get<game::NetworkIdentity>(touched.ship);
  if (item == nullptr || look == nullptr || ship == nullptr) {
    return std::nullopt;
  }
  const ConnectedPlayer* const player = roster.findByShip(ship->networkId);
  if (player == nullptr) {
    return std::nullopt;
  }
  return LookItemClaim{.item = touched.item,
                       .itemId = item->networkId,
                       .lookNumber = look->number,
                       .playerNumber = player->playerNumber,
                       .shipId = ship->networkId};
}

/** @returns Whether the pilot of @p claim is still in @p roster and still
 * flies the ship that touched the item: a ship destroyed in the same tick
 * resets the ship of its player. */
bool isStillFlown(PlayerRoster& roster, const LookItemClaim& claim) {
  const ConnectedPlayer* const player = roster.findByNumber(claim.playerNumber);
  return player != nullptr && player->shipEntityId == claim.shipId;
}

}  // namespace

LookItemKeeper::LookItemKeeper(LookItemKeeperParts parts)
    : events_(parts.events),
      world_(parts.world),
      roster_(parts.roster),
      send_(std::move(parts.send)),
      subscription_(events_.subscribe<game::LookItemTouched>(
          [this](const game::LookItemTouched& touched) {
            onTouched(touched);
          })) {}

LookItemKeeper::~LookItemKeeper() { events_.unsubscribe(subscription_); }

void LookItemKeeper::onTick() {
  std::ranges::sort(claims_,
                    [](const LookItemClaim& left, const LookItemClaim& right) {
                      return std::pair{left.itemId, left.playerNumber} <
                             std::pair{right.itemId, right.playerNumber};
                    });
  const std::span<const LookItemClaim> all{claims_};
  std::size_t first = 0;
  while (first < all.size()) {
    std::size_t last = first;
    while (last < all.size() && all[last].itemId == all[first].itemId) {
      ++last;
    }
    giveItem(all.subspan(first, last - first));
    first = last;
  }
  claims_.clear();
}

void LookItemKeeper::onTouched(const game::LookItemTouched& touched) {
  const std::optional<LookItemClaim> claim = claimOf(world_, roster_, touched);
  if (claim.has_value()) {
    claims_.push_back(*claim);
  }
}

void LookItemKeeper::giveItem(std::span<const LookItemClaim> claims) {
  const auto winner =
      std::ranges::find_if(claims, [this](const LookItemClaim& claim) {
        return isStillFlown(roster_, claim);
      });
  if (winner == claims.end() ||
      world_.components().get<game::LookItem>(winner->item) == nullptr) {
    return;
  }
  world_.components().destroy(winner->item);
  events_.publish(game::EntityDestruction{
      .networkId = winner->itemId, .cause = game::DestructionCause::Removed});
  ConnectedPlayer* const player = roster_.findByNumber(winner->playerNumber);
  guaranteeToPlayer(*player, send_,
                    network::encodedMessage(
                        network::LookItemWon{.item = winner->lookNumber}));
}

}  // namespace rtype::server
