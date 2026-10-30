#include "StartBarrier.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "GameInstanceConstants.hpp"
#include "GameStarted.hpp"
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

namespace {

bool isStillLoading(const ConnectedPlayer* player) {
  return !player->away && !player->loaded;
}

}  // namespace

StartBarrier::StartBarrier(PlayerRoster& roster,
                           std::function<void(network::OutgoingDatagram)> send)
    : roster_(roster),
      send_(std::move(send)),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {}

void StartBarrier::open() {
  waitedTicks_ = 0;
  for (ConnectedPlayer* const player : roster_.connected()) {
    player->loaded = false;
    send_(sessionDatagram(*player, network::GameStarted{}));
  }
}

bool StartBarrier::advance() {
  ++waitedTicks_;
  const ConnectedPlayers players = roster_.connected();
  const auto stillLoading =
      static_cast<std::size_t>(std::ranges::count_if(players, isStillLoading));
  if (stillLoading == 0) {
    return true;
  }
  if (waitedTicks_ >= START_BARRIER_TICKS) {
    logger_.warn("starting without ", stillLoading,
                 " player(s) who did not load the game in time");
    return true;
  }
  if (waitedTicks_ % GAME_STARTED_REPEAT_TICKS == 0) {
    for (ConnectedPlayer* const player : players) {
      if (isStillLoading(player)) {
        send_(sessionDatagram(*player, network::GameStarted{}));
      }
    }
  }
  return false;
}

}  // namespace rtype::server
