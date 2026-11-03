#include "DatagramSorterFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "PacketHeader.hpp"
#include "ServerDatagrams.hpp"
#include "ServerTestConstants.hpp"

rtype::network::Endpoint player() {
  return rtype::network::Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                                  .port = PLAYER_PORT};
}

rtype::network::Endpoint stranger() {
  return rtype::network::Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                                  .port = STRANGER_PORT};
}

rtype::network::Endpoint secondPlayer() {
  return rtype::network::Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                                  .port = SECOND_PLAYER_PORT};
}

rtype::network::PacketHeader playerHeader(std::uint64_t sessionToken) {
  rtype::network::PacketHeader header;
  header.sessionToken = sessionToken;
  return header;
}

std::vector<std::byte> playerReady(std::uint64_t sessionToken) {
  return readyDatagram(playerHeader(sessionToken));
}
