#include "ReadyKeeper.hpp"
#include <algorithm>
#include <functional>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "GameLoaded.hpp"
#include "MessageOrigin.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"

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
  player->ready = message.ready;
}

void ReadyKeeper::apply(const network::MessageOrigin& origin,
                        [[maybe_unused]] const network::GameLoaded& message) {
  ConnectedPlayer* const player = roster_.find(origin.sender);
  if (player != nullptr) {
    player->loaded = true;
  }
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
