#include "FarewellSender.hpp"
#include <cstddef>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "GameEventConstants.hpp"
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "ModuleConstants.hpp"
#include "PlayerRoster.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

FarewellSender::FarewellSender(IDatagramSender& sender, Pause pause)
    : sender_(&sender), pause_(std::move(pause)) {}

void FarewellSender::sayGoodbyeTo(const Games& games) {
  if (queueOneCopy(games) == 0) {
    return;
  }
  for (std::size_t copy = 1; copy < REPEATED_MESSAGE_COPIES; ++copy) {
    pause_(REPEATED_MESSAGE_INTERVAL);
    static_cast<void>(queueOneCopy(games));
  }
  pause_(FAREWELL_FLUSH_WAIT);
}

std::size_t FarewellSender::queueOneCopy(const Games& games) {
  std::size_t playerCount = 0;
  for (const auto& game : games.all()) {
    PlayerRoster& roster = game->roster();
    for (const network::Endpoint& endpoint : roster.endpoints()) {
      ConnectedPlayer* const player = roster.find(endpoint);
      if (player == nullptr) {
        continue;
      }
      sender_->queueDatagram(
          sessionDatagram(*player, network::Disconnection{}));
      ++playerCount;
    }
  }
  return playerCount;
}

}  // namespace rtype::server
