#include "NetworkThreadFixtures.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"
#include "NetworkTestConstants.hpp"
#include "NetworkThread.hpp"
#include "NetworkThreadTestConstants.hpp"

rtype::network::Endpoint loopbackOnPort(std::uint16_t port) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = port};
}

std::vector<std::byte> exchangedBytes() {
  return {EXCHANGED_BYTES.begin(), EXCHANGED_BYTES.end()};
}

std::vector<rtype::network::IncomingDatagram> drainUntilReceived(
    rtype::client::NetworkThread& networkThread) {
  const auto deadline = std::chrono::steady_clock::now() + DATAGRAM_TIMEOUT;
  std::vector<rtype::network::IncomingDatagram> received;
  networkThread.drainReceived(received);
  while (received.empty() && std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(DRAIN_INTERVAL);
    networkThread.drainReceived(received);
  }
  return received;
}
