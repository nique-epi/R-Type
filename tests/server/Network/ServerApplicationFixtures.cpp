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
#include "ServerApplication.hpp"
#include "ServerDatagrams.hpp"
#include "ServerTestConstants.hpp"
#include "UdpSocket.hpp"

SortedReadyState sortReadyStateThenRequest(
    const std::function<void(rtype::server::ServerApplication&,
                             const rtype::network::Endpoint&)>& prepare,
    const std::vector<std::byte>& readyState) {
  std::promise<void> requestArrived;
  const std::future<void> arrived = requestArrived.get_future();
  std::atomic<bool> delivered{false};
  std::atomic<bool> readyStateSeen{false};
  rtype::server::ServerApplication server{
      loopbackEndpoint(rtype::network::ANY_PORT),
      [&](const rtype::server::ServerApplication::ReceivedDatagrams&
              datagrams) {
        for (const auto* list :
             {&datagrams.players, &datagrams.connectionRequests}) {
          for (const rtype::network::IncomingDatagram& datagram : *list) {
            if (datagram.payload == readyState) {
              readyStateSeen = true;
            }
          }
        }
        if (!datagrams.connectionRequests.empty() &&
            !delivered.exchange(true)) {
          requestArrived.set_value();
        }
      }};
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
      .readyStateReachedATick = readyStateSeen};
}
