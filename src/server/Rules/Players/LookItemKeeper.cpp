#include "LookItemKeeper.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "ItemClaim.hpp"
#include "ItemClaims.hpp"
#include "ItemTouch.hpp"
#include "LookItem.hpp"
#include "LookItemKeeperParts.hpp"
#include "LookItemTouched.hpp"
#include "LookItemWon.hpp"
#include "MessageDelivery.hpp"
#include "MessagePayload.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @brief Adds @p lookNumber to the items @p player won in this game, unless
 * it already won that item. */
void keepWon(ConnectedPlayer& player, std::uint8_t lookNumber) {
  if (std::ranges::find(player.wonLookNumbers, lookNumber) ==
      player.wonLookNumbers.end()) {
    player.wonLookNumbers.push_back(lookNumber);
  }
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
  for (const ItemClaim& winner :
       winningClaims(std::exchange(claims_, {}), roster_)) {
    giveItem(winner);
  }
}

void LookItemKeeper::onTouched(const game::LookItemTouched& touched) {
  const std::optional<ItemClaim> claim = itemClaimOf(
      world_, roster_, ItemTouch{.ship = touched.ship, .item = touched.item});
  if (claim.has_value()) {
    claims_.push_back(*claim);
  }
}

void LookItemKeeper::giveItem(const ItemClaim& winner) {
  const auto* const look = world_.components().get<game::LookItem>(winner.item);
  if (look == nullptr) {
    return;
  }
  const std::uint8_t lookNumber = look->number;
  world_.components().destroy(winner.item);
  events_.publish(game::EntityDestruction{
      .networkId = winner.itemId, .cause = game::DestructionCause::Removed});
  ConnectedPlayer* const player = roster_.findByNumber(winner.playerNumber);
  keepWon(*player, lookNumber);
  guaranteeToPlayer(
      *player, send_,
      network::encodedMessage(network::LookItemWon{.item = lookNumber}));
}

}  // namespace rtype::server
