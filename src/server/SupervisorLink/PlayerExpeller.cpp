#include "PlayerExpeller.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include "ConnectedPlayer.hpp"
#include "Expulsion.hpp"
#include "IDatagramSender.hpp"
#include "ModuleConstants.hpp"
#include "PlayerRoster.hpp"
#include "ServerConstants.hpp"
#include "SessionDatagram.hpp"
#include "WireTypes.hpp"

namespace rtype::server {

PlayerExpeller::PlayerExpeller() : logger_(std::string{SERVER_LOGGER_NAME}) {}

void PlayerExpeller::expel(IDatagramSender& sender, PlayerRoster& roster,
                           std::string_view nickname) const {
  for (ConnectedPlayer* const player : roster.connected()) {
    if (!player->expelled &&
        network::sameNickname(player->nickname, nickname)) {
      tellExpelled(sender, *player);
    }
  }
}

void PlayerExpeller::tellExpelled(IDatagramSender& sender,
                                  ConnectedPlayer& player) const {
  for (std::size_t copy = 0; copy < FAREWELL_COPIES; ++copy) {
    sender.queueDatagram(sessionDatagram(
        player,
        network::Expulsion{.reason = network::ExpulsionReason::ByTheHost}));
  }
  player.expelled = true;
  logger_.info("player ", static_cast<unsigned int>(player.playerNumber), " (",
               player.nickname,
               ") expelled by the host: its session closes at the next tick");
}

}  // namespace rtype::server
