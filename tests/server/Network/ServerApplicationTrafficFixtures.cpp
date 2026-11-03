#include "ServerApplicationTrafficFixtures.hpp"
#include <chrono>
#include <cstddef>
#include <functional>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "ListenOnFixtures.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerApplication.hpp"
#include "ServerApplicationFixtures.hpp"
#include "ServerTestConstants.hpp"
#include "TrafficReport.hpp"
#include "TrafficTotals.hpp"
#include "UdpSocket.hpp"

using rtype::network::ANY_PORT;
using rtype::network::Endpoint;
using rtype::network::NetworkConditions;
using rtype::network::TrafficReport;
using rtype::network::TrafficTotals;
using rtype::server::ServerApplication;

namespace {

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
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks()};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackEndpoint(ANY_PORT)};
  const Endpoint bound = server.open();
  prepare(server, loopbackEndpoint(client.localEndpoint().port));
  const std::jthread runner{[&server] { server.run(); }};
  client.send(loopbackEndpoint(bound.port), playerReady());
  clientContext.run();
  waitUntil(server, [](const TrafficTotals& totals) {
    return totals.received.count > 0;
  });
  server.stop();
  return server.traffic().report();
}

TrafficReport trafficAfterAQueuedDatagram(const NetworkConditions& conditions) {
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks(),
                           conditions};
  const Endpoint bound = server.open();
  const std::jthread runner{[&server] { server.run(); }};
  server.queueDatagram(rtype::network::OutgoingDatagram{
      .destination = loopbackEndpoint(bound.port),
      .payload = std::vector<std::byte>(1)});
  waitUntil(server,
            [](const TrafficTotals& totals) { return totals.sent.count > 0; });
  server.stop();
  return server.traffic().report();
}
