#include "PlayerExpeller.hpp"
#include <cstddef>
#include <functional>
#include <string>
#include "ConnectedPlayer.hpp"
#include "Expulsion.hpp"
#include "ExpulsionRule.hpp"
#include "ModuleConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SessionConstants.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

PlayerExpeller::PlayerExpeller() : logger_(std::string{SESSIONS_LOGGER_NAME}) {}

void PlayerExpeller::expel(
    const std::function<void(network::OutgoingDatagram)>& send,
    PlayerRoster& roster, const ExpulsionRule& rule) const {
  for (ConnectedPlayer* const player : roster.connected()) {
    if (!player->expulsion.has_value() && rule.chosen(*player)) {
      tellExpelled(send, *player, rule.reason);
    }
  }
}

void PlayerExpeller::tellExpelled(
    const std::function<void(network::OutgoingDatagram)>& send,
    ConnectedPlayer& player, network::ExpulsionReason reason) const {
  for (std::size_t copy = 0; copy < FAREWELL_COPIES; ++copy) {
    send(sessionDatagram(player, network::Expulsion{.reason = reason}));
  }
  player.expulsion = reason;
  logger_.info("player ", static_cast<unsigned int>(player.playerNumber), " (",
               player.nickname,
               ") expelled by the host: its session closes at the next tick");
}

}  // namespace rtype::server
