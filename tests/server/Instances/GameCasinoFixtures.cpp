#include "GameCasinoFixtures.hpp"
#include <cstddef>
#include <vector>
#include "CasinoPlace.hpp"
#include "DatagramCodec.hpp"
#include "GameInstanceFixtures.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

network::IncomingDatagram casinoPlaceFrom(std::size_t index) {
  return network::IncomingDatagram{
      .sender = playerEndpoint(index),
      .payload = network::DatagramCodec::encode(
          firstSessionHeader(index),
          network::CasinoPlace{.place = network::TablePlace::AtTable})};
}

std::vector<network::LobbyState> lobbyStatesIn(
    const std::vector<network::OutgoingDatagram>& datagrams) {
  std::vector<network::LobbyState> states;
  for (const network::OutgoingDatagram& datagram : datagrams) {
    for (const network::LobbyState& state :
         messagesIn<network::LobbyState>(datagram)) {
      states.push_back(state);
    }
  }
  return states;
}

}  // namespace rtype::server::testing
