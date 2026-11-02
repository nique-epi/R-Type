#include "DatagramRouterFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "DatagramRouterTestConstants.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::client::testing {

network::Endpoint gameServer() {
  return network::Endpoint{.address = std::string{GAME_SERVER_ADDRESS},
                           .port = GAME_SERVER_PORT};
}

network::Endpoint masterServer() {
  return network::Endpoint{.address = std::string{GAME_SERVER_ADDRESS},
                           .port = MASTER_SERVER_PORT};
}

network::Endpoint stranger() {
  return network::Endpoint{.address = std::string{STRANGER_ADDRESS},
                           .port = STRANGER_PORT};
}

network::IncomingDatagram datagramFrom(const network::Endpoint& sender,
                                       std::byte tag) {
  return network::IncomingDatagram{.sender = sender, .payload = {tag}};
}

network::OutgoingDatagram datagramTo(const network::Endpoint& destination,
                                     std::byte tag) {
  return network::OutgoingDatagram{.destination = destination,
                                   .payload = {tag}};
}

std::vector<network::IncomingDatagram> datagramsFrom(
    const network::Endpoint& sender, const std::vector<std::byte>& tags) {
  std::vector<network::IncomingDatagram> datagrams;
  datagrams.reserve(tags.size());
  for (const std::byte tag : tags) {
    datagrams.push_back(datagramFrom(sender, tag));
  }
  return datagrams;
}

}  // namespace rtype::client::testing
