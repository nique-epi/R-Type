#include "SimulatorFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "SimulatorTestConstants.hpp"

rtype::network::Endpoint peer() {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = PEER_PORT};
}

std::vector<std::byte> crossingBytes() {
  return {CROSSING_BYTES.begin(), CROSSING_BYTES.end()};
}
