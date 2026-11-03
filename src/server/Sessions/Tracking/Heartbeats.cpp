#include "Heartbeats.hpp"
#include <functional>
#include "ConnectedPlayer.hpp"
#include "Heartbeat.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SessionConstants.hpp"
#include "SessionDatagram.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

void addTimeSinceSent(PlayerRoster& roster, engine::Duration elapsed) {
  for (ConnectedPlayer* const player : roster.connected()) {
    player->sinceSent += elapsed;
  }
}

void sendHeartbeats(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send) {
  for (ConnectedPlayer* const player : roster.connected()) {
    if (player->sinceSent >= SILENCE_BEFORE_HEARTBEAT) {
      send(sessionDatagram(*player, network::Heartbeat{}));
    }
  }
}

}  // namespace rtype::server
