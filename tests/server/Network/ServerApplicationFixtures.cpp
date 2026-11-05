#include "ServerApplicationFixtures.hpp"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <span>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "ListenOnFixtures.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "ReceivedDatagrams.hpp"
#include "ServerApplication.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "TickHandler.hpp"
#include "UdpSocket.hpp"

using rtype::network::ANY_PORT;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::server::ReceivedDatagrams;
using rtype::server::ServerApplication;
using rtype::server::TickHandler;

namespace {

/** @brief What the ticks of a server saw, as readyStateWatcher() notes it. */
struct ReadyStateWatch {
  std::promise<void> requestArrived;
  std::atomic<bool> delivered{false};
  std::atomic<bool> readyStateSeen{false};
};

/**
 * @returns A tick handler that notes in @p watch whether a tick was handed
 * @p readyState, and fulfils its promise at the first connection request.
 * Both must outlive the server.
 */
TickHandler readyStateWatcher(ReadyStateWatch& watch,
                              const std::vector<std::byte>& readyState) {
  return [&watch, &readyState](const ReceivedDatagrams& datagrams) {
    for (const auto* list :
         {&datagrams.players, &datagrams.connectionRequests}) {
      for (const IncomingDatagram& datagram : *list) {
        if (datagram.payload == readyState) {
          watch.readyStateSeen = true;
        }
      }
    }
    if (!datagrams.connectionRequests.empty() &&
        !watch.delivered.exchange(true)) {
      watch.requestArrived.set_value();
    }
  };
}

/** @brief What the ticks of a server saw of one client, as requestWatcher()
 * notes it. */
struct RequestWatch {
  std::promise<void> requestArrived;
  std::atomic<bool> delivered{false};
  std::atomic<std::uint16_t> clientPort{0};
};

/**
 * @returns A tick handler that fulfils the promise of @p watch at the first
 * connection request of the client on its port. @p watch must outlive the
 * server.
 */
TickHandler requestWatcher(RequestWatch& watch) {
  return [&watch](const ReceivedDatagrams& datagrams) {
    for (const IncomingDatagram& request : datagrams.connectionRequests) {
      if (request.sender.port == watch.clientPort &&
          !watch.delivered.exchange(true)) {
        watch.requestArrived.set_value();
      }
    }
  };
}

/** @brief Has @p socket send @p count connection requests to @p server. */
void sendRequests(rtype::network::UdpSocket& socket, const Endpoint& server,
                  std::size_t count) {
  for (std::size_t sent = 0; sent < count; ++sent) {
    socket.send(server, requestDatagram());
  }
}

}  // namespace

bool requestOfAClientReachesATickThroughAFlood() {
  RequestWatch watch;
  const std::future<void> arrived = watch.requestArrived.get_future();
  ServerApplication server{loopbackEndpoint(ANY_PORT), requestWatcher(watch)};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket flooder{clientContext, loopbackEndpoint(ANY_PORT)};
  rtype::network::UdpSocket client{clientContext, loopbackEndpoint(ANY_PORT)};
  watch.clientPort = client.localEndpoint().port;
  const Endpoint bound = loopbackEndpoint(server.open().port);
  const std::jthread runner{[&server] { server.run(); }};
  sendRequests(flooder, bound, FLOOD_REQUESTS_AROUND_A_CLIENT);
  client.send(bound, requestDatagram());
  sendRequests(flooder, bound, FLOOD_REQUESTS_AROUND_A_CLIENT);
  clientContext.run();
  const std::future_status status = arrived.wait_for(THREAD_TIMEOUT);
  server.stop();
  return status == std::future_status::ready;
}

rtype::network::DatagramHandler ignoringWhatItReceives() {
  return []([[maybe_unused]] const rtype::network::Endpoint& sender,
            [[maybe_unused]] std::span<const std::byte> bytes) {};
}

rtype::server::TickHandler admittingFirstRequester(AdmissionWatch& watch) {
  return [&watch](const rtype::server::ReceivedDatagrams& datagrams) {
    if (!datagrams.connectionRequests.empty() &&
        !watch.admissionDone.exchange(true)) {
      watch.server->admit(datagrams.connectionRequests.front().sender,
                          rtype::server::testing::SESSION_TOKEN);
      watch.admission.set_value();
    }
    if (!datagrams.players.empty() && !watch.delivered.exchange(true)) {
      watch.firstDatagram.set_value(datagrams.players.front());
    }
  };
}

rtype::server::TickHandler queueingOnce(QueuedOnce& once) {
  return
      [&once](
          [[maybe_unused]] const rtype::server::ReceivedDatagrams& datagrams) {
        if (!once.queued.exchange(true)) {
          once.server->queueDatagram(once.datagram);
        }
      };
}

SortedReadyState sortReadyStateThenRequest(
    const std::function<void(ServerApplication&, const Endpoint&)>& prepare,
    const std::vector<std::byte>& readyState) {
  ReadyStateWatch watch;
  const std::future<void> arrived = watch.requestArrived.get_future();
  ServerApplication server{loopbackEndpoint(ANY_PORT),
                           readyStateWatcher(watch, readyState)};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackEndpoint(ANY_PORT)};
  const Endpoint bound = server.open();
  prepare(server, loopbackEndpoint(client.localEndpoint().port));
  const std::jthread runner{[&server] { server.run(); }};
  client.send(loopbackEndpoint(bound.port), readyState);
  client.send(loopbackEndpoint(bound.port), requestDatagram());
  clientContext.run();
  const std::future_status status = arrived.wait_for(THREAD_TIMEOUT);
  server.stop();
  return SortedReadyState{
      .requestReachedATick = status == std::future_status::ready,
      .readyStateReachedATick = watch.readyStateSeen};
}
