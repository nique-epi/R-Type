#include "GamePlayers.hpp"
#include <cstddef>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameInstance.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::server::testing::playerEndpoint;

rtype::server::ConnectedPlayer& seatPlayerIn(rtype::server::GameInstance& game,
                                             std::size_t index) {
  rtype::server::ConnectedPlayer* const player =
      game.roster().add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                        PLAYER_TOKEN_BASE + index);
  game.onPlayerJoined(*player);
  return *player;
}

rtype::network::IncomingDatagram disconnectionFrom(std::size_t index) {
  rtype::network::PacketHeader header;
  header.sessionToken = PLAYER_TOKEN_BASE + index;
  return rtype::network::IncomingDatagram{
      .sender = playerEndpoint(index),
      .payload = disconnectionDatagram(header)};
}
