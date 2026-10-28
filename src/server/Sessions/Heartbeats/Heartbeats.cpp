#include "Heartbeats.hpp"
#include <functional>
#include "ConnectedPlayer.hpp"
#include "Heartbeat.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SessionConstants.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

void sendHeartbeats(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send) {
  for (ConnectedPlayer* const player : roster.connected()) {
    if (player->ticksSinceSent >= TICKS_PER_HEARTBEAT) {
      send(sessionDatagram(*player, network::Heartbeat{}));
    }
    ++player->ticksSinceSent;
  }
}

}  // namespace rtype::server
