#include "ServerApplicationFixtures.hpp"
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <span>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "ListenOnFixtures.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "ServerApplication.hpp"
#include "ServerDatagrams.hpp"
#include "ServerTestConstants.hpp"
#include "UdpSocket.hpp"

namespace {

/** @returns Whether one of @p datagrams carries @p payload. */
bool holdsPayload(std::span<const rtype::network::IncomingDatagram> datagrams,
                  const std::vector<std::byte>& payload) {
  return std::ranges::any_of(
      datagrams, [&payload](const rtype::network::IncomingDatagram& datagram) {
        return datagram.payload == payload;
      });
}

/** @brief What the ticks of a server saw: the first connection request, and
 * a ReadyState among any of the datagrams. */
struct TickSightings {
  std::promise<void> requestArrived;
  std::atomic<bool> requestSeen{false};
  std::atomic<bool> readyStateSeen{false};
};

/**
 * @returns A tick handler that writes into @p sightings what reaches it:
 * @p readyState among the datagrams of the players or the requests, and the
 * first connection request. @p sightings and @p readyState must outlive it.
 */
rtype::server::ServerApplication::TickHandler sightingTicks(
    TickSightings& sightings, const std::vector<std::byte>& readyState) {
  return [&sightings, &readyState](
             const rtype::server::ServerApplication::ReceivedDatagrams&
                 datagrams) {
    if (holdsPayload(datagrams.players, readyState) ||
        holdsPayload(datagrams.connectionRequests, readyState)) {
      sightings.readyStateSeen = true;
    }
    if (!datagrams.connectionRequests.empty() &&
        !sightings.requestSeen.exchange(true)) {
      sightings.requestArrived.set_value();
    }
  };
}

}  // namespace

SortedReadyState sortReadyStateThenRequest(
    const std::function<void(rtype::server::ServerApplication&,
                             const rtype::network::Endpoint&)>& prepare,
    const std::vector<std::byte>& readyState) {
  TickSightings sightings;
  const std::future<void> arrived = sightings.requestArrived.get_future();
  rtype::server::ServerApplication server{
      loopbackEndpoint(rtype::network::ANY_PORT),
      sightingTicks(sightings, readyState)};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext,
                                   loopbackEndpoint(rtype::network::ANY_PORT)};
  server.open();
  prepare(server, loopbackEndpoint(client.localEndpoint().port));
  const std::jthread runner{[&server] { server.run(); }};
  client.send(loopbackEndpoint(server.localEndpoint().port), readyState);
  client.send(loopbackEndpoint(server.localEndpoint().port), requestDatagram());
  clientContext.run();
  const std::future_status status = arrived.wait_for(THREAD_TIMEOUT);
  server.stop();
  return SortedReadyState{
      .requestReachedATick = status == std::future_status::ready,
      .readyStateReachedATick = sightings.readyStateSeen};
}
