#include "DatagramRelayFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "DatagramRelayTestConstants.hpp"
#include "Endpoint.hpp"

rtype::network::Endpoint peerOnPort(std::uint16_t port) {
  return rtype::network::Endpoint{.address = std::string{PEER_ADDRESS},
                                  .port = port};
}

std::vector<std::byte> receivedBytes() {
  std::vector<std::byte> bytes(RELAY_PAYLOAD_SIZE, RECEIVED_BYTE);
  return bytes;
}
