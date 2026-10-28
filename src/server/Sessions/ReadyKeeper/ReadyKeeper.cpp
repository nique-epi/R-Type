#include "ReadyKeeper.hpp"
#include <algorithm>
#include <functional>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "MessageOrigin.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"
#include "SequenceOrder.hpp"

namespace rtype::server {

ReadyKeeper::ReadyKeeper(PlayerRoster& roster,
                         std::function<bool()> readyAccepted)
    : roster_(roster), readyAccepted_(std::move(readyAccepted)) {}

void ReadyKeeper::apply(const network::MessageOrigin& origin,
                        const network::ReadyState& message) {
  ConnectedPlayer* const player = roster_.find(origin.sender);
  if (!readyAccepted_() || player == nullptr) {
    return;
  }
  if (player->newestReceivedSequence.has_value() &&
      !network::isNewerSequence(origin.header.sequence,
                                *player->newestReceivedSequence)) {
    return;
  }
  player->newestReceivedSequence = origin.header.sequence;
  player->ready = message.ready;
}

bool ReadyKeeper::everyPlayerIsReady() {
  const ConnectedPlayers players = roster_.connected();
  const auto isPresent = [](const ConnectedPlayer* player) {
    return !player->away;
  };
  return std::ranges::any_of(players, isPresent) &&
         std::ranges::all_of(players,
                             [&isPresent](const ConnectedPlayer* player) {
                               return !isPresent(player) || player->ready;
                             });
}

}  // namespace rtype::server
