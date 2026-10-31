#include "SimulatorFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "SimulatorTestConstants.hpp"

rtype::network::Endpoint loopbackOnPort(std::uint16_t port) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = port};
}

rtype::network::Endpoint peer() { return loopbackOnPort(PEER_PORT); }

std::vector<std::byte> crossingBytes() {
  return {CROSSING_BYTES.begin(), CROSSING_BYTES.end()};
}
