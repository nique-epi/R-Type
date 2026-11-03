#include "ServerApplicationFixtures.hpp"
#include <atomic>
#include <cstddef>
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
#include "PacketHeader.hpp"
#include "ServerApplication.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "UdpSocket.hpp"

using rtype::network::ANY_PORT;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::server::ServerApplication;

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
ServerApplication::TickHandler readyStateWatcher(
    ReadyStateWatch& watch, const std::vector<std::byte>& readyState) {
  return [&watch,
          &readyState](const ServerApplication::ReceivedDatagrams& datagrams) {
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

}  // namespace

std::vector<std::byte> playerReady() {
  rtype::network::PacketHeader header;
  header.sessionToken = rtype::server::testing::SESSION_TOKEN;
  return readyDatagram(header);
}

rtype::network::DatagramHandler ignoringWhatItReceives() {
  return []([[maybe_unused]] const rtype::network::Endpoint& sender,
            [[maybe_unused]] std::span<const std::byte> bytes) {};
}

rtype::server::ServerApplication::TickHandler admittingFirstRequester(
    AdmissionWatch& watch) {
  return [&watch](const rtype::server::ServerApplication::ReceivedDatagrams&
                      datagrams) {
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

rtype::server::ServerApplication::TickHandler queueingOnce(QueuedOnce& once) {
  return [&once]([[maybe_unused]] const rtype::server::ServerApplication::
                     ReceivedDatagrams& datagrams) {
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
