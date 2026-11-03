#include "ServerApplicationTrafficFixtures.hpp"
#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "PacketHeader.hpp"
#include "ServerApplication.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "TrafficReport.hpp"
#include "TrafficTotals.hpp"
#include "UdpSocket.hpp"

using rtype::network::Endpoint;
using rtype::server::ServerApplication;

namespace {

Endpoint loopbackOnPort(std::uint16_t port) {
  return Endpoint{.address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
                  .port = port};
}

/** @brief Waits until @p server counted a datagram, or for THREAD_TIMEOUT. */
void waitForADatagram(ServerApplication& server) {
  const auto deadline = std::chrono::steady_clock::now() + THREAD_TIMEOUT;
  while (rtype::network::trafficTotalsOf(server.traffic().report())
                 .received.count == 0 &&
         std::chrono::steady_clock::now() < deadline) {
    std::this_thread::yield();
  }
}

}  // namespace

rtype::network::TrafficReport trafficAfterAReadyState(
    const std::function<void(ServerApplication&, const Endpoint&)>& prepare) {
  ServerApplication server{
      loopbackOnPort(rtype::network::ANY_PORT),
      []([[maybe_unused]] const ServerApplication::ReceivedDatagrams&
             received) {}};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext,
                                   loopbackOnPort(rtype::network::ANY_PORT)};
  server.open();
  prepare(server, loopbackOnPort(client.localEndpoint().port));
  const std::jthread runner{[&server] { server.run(); }};
  rtype::network::PacketHeader header;
  header.sessionToken = rtype::server::testing::SESSION_TOKEN;
  client.send(loopbackOnPort(server.localEndpoint().port),
              readyDatagram(header));
  clientContext.run();
  waitForADatagram(server);
  server.stop();
  return server.traffic().report();
}
