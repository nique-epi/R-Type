#include "ServerApplicationFixtures.hpp"
#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "ListenOnFixtures.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "PacketHeader.hpp"
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

}  // namespace

std::vector<std::byte> playerReady() {
  rtype::network::PacketHeader header;
  header.sessionToken = rtype::server::testing::SESSION_TOKEN;
  return readyDatagram(header);
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
