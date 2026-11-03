#include "ServerApplicationTrafficFixtures.hpp"
#include <chrono>
#include <cstddef>
#include <functional>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerApplication.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficReport.hpp"
#include "TrafficTotals.hpp"
#include "UdpSocket.hpp"

using rtype::network::ANY_PORT;
using rtype::network::Endpoint;
using rtype::network::NetworkConditions;
using rtype::network::TrafficReport;
using rtype::network::TrafficTotals;
using rtype::network::testing::loopbackOnPort;
using rtype::server::ServerApplication;

namespace {

ServerApplication::TickHandler ignoringDatagrams() {
  return []([[maybe_unused]] const ServerApplication::ReceivedDatagrams&
                received) {};
}

/** @brief Waits until @p counted holds for what @p server counted, or for
 * THREAD_TIMEOUT. */
void waitUntil(ServerApplication& server,
               const std::function<bool(const TrafficTotals&)>& counted) {
  const auto deadline = std::chrono::steady_clock::now() + THREAD_TIMEOUT;
  while (!counted(trafficTotalsOf(server.traffic().report())) &&
         std::chrono::steady_clock::now() < deadline) {
    std::this_thread::yield();
  }
}

}  // namespace

TrafficReport trafficAfterAReadyState(
    const std::function<void(ServerApplication&, const Endpoint&)>& prepare) {
  ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackOnPort(ANY_PORT)};
  server.open();
  prepare(server, loopbackOnPort(client.localEndpoint().port));
  const std::jthread runner{[&server] { server.run(); }};
  rtype::network::PacketHeader header;
  header.sessionToken = rtype::server::testing::SESSION_TOKEN;
  client.send(loopbackOnPort(server.localEndpoint().port),
              readyDatagram(header));
  clientContext.run();
  waitUntil(server, [](const TrafficTotals& totals) {
    return totals.received.count > 0;
  });
  server.stop();
  return server.traffic().report();
}

TrafficReport trafficAfterAQueuedDatagram(const NetworkConditions& conditions) {
  ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams(),
                           conditions};
  server.open();
  const std::jthread runner{[&server] { server.run(); }};
  server.queueDatagram(rtype::network::OutgoingDatagram{
      .destination = loopbackOnPort(server.localEndpoint().port),
      .payload = std::vector<std::byte>(1)});
  waitUntil(server,
            [](const TrafficTotals& totals) { return totals.sent.count > 0; });
  server.stop();
  return server.traffic().report();
}
