#include "FarewellSender.hpp"
#include <cstddef>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "FarewellPause.hpp"
#include "IDatagramSender.hpp"
#include "ModuleConstants.hpp"
#include "PlayerRoster.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

FarewellSender::FarewellSender(IDatagramSender& sender, FarewellPause pause)
    : sender_(&sender), pause_(std::move(pause)) {}

void FarewellSender::sayGoodbyeTo(PlayerRoster& roster) {
  if (queueOneCopy(roster) == 0) {
    return;
  }
  for (std::size_t copy = 1; copy < FAREWELL_COPIES; ++copy) {
    pause_(FAREWELL_COPY_INTERVAL);
    static_cast<void>(queueOneCopy(roster));
  }
  pause_(FAREWELL_FLUSH_WAIT);
}

std::size_t FarewellSender::queueOneCopy(PlayerRoster& roster) {
  std::size_t playerCount = 0;
  for (const network::Endpoint& endpoint : roster.endpoints()) {
    ConnectedPlayer* const player = roster.find(endpoint);
    if (player == nullptr) {
      continue;
    }
    sender_->queueDatagram(sessionDatagram(*player, network::Disconnection{}));
    ++playerCount;
  }
  return playerCount;
}

}  // namespace rtype::server
