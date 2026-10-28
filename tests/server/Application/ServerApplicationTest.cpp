#include <gtest/gtest.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <future>
#include <span>
#include <string>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerApplication.hpp"
#include "ServerDatagrams.hpp"
#include "ServerException.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "TimeConstants.hpp"
#include "UdpSocket.hpp"

using rtype::network::ANY_PORT;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::LOOPBACK_IPV4_ADDRESS;
using rtype::server::ServerApplication;
using rtype::server::testing::SESSION_TOKEN;

namespace {

/**
 * @brief What a test tick handler throws to make the server stop.
 */
class TickFailure final : public std::exception {};

Endpoint loopbackOnPort(std::uint16_t port) {
  return Endpoint{.address = std::string{LOOPBACK_IPV4_ADDRESS}, .port = port};
}

/** @returns A ReadyState carrying SESSION_TOKEN, as an admitted player sends.
 */
std::vector<std::byte> playerReady() {
  rtype::network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  return readyDatagram(header);
}

ServerApplication::TickHandler ignoringDatagrams() {
  return []([[maybe_unused]] const ServerApplication::ReceivedDatagrams&
                received) {};
}

/** @brief What the ticks of a server were handed in
 * sortReadyStateThenRequest().
 */
struct SortedReadyState {
  bool requestReachedATick{};
  bool readyStateReachedATick{};
};

/**
 * @brief Runs a server on the loopback address, lets @p prepare admit or forget
 * the address of a client before the server starts, then has that client send
 * @p readyState, then a connection request.
 * @returns Whether a tick was handed the request within THREAD_TIMEOUT, and
 * whether any tick was handed @p readyState, among the datagrams of the players
 * or the requests.
 */
SortedReadyState sortReadyStateThenRequest(
    const std::function<void(ServerApplication&, const Endpoint&)>& prepare,
    const std::vector<std::byte>& readyState) {
  std::promise<void> requestArrived;
  const std::future<void> arrived = requestArrived.get_future();
  std::atomic<bool> delivered{false};
  std::atomic<bool> readyStateSeen{false};
  ServerApplication server{
      loopbackOnPort(ANY_PORT),
      [&](const ServerApplication::ReceivedDatagrams& datagrams) {
        for (const auto* list :
             {&datagrams.players, &datagrams.connectionRequests}) {
          for (const IncomingDatagram& datagram : *list) {
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
  rtype::network::UdpSocket client{clientContext, loopbackOnPort(ANY_PORT)};
  server.open();
  prepare(server, loopbackOnPort(client.localEndpoint().port));
  const std::jthread runner{[&server] { server.run(); }};
  client.send(loopbackOnPort(server.localEndpoint().port), readyState);
  client.send(loopbackOnPort(server.localEndpoint().port), requestDatagram());
  clientContext.run();
  const std::future_status status = arrived.wait_for(THREAD_TIMEOUT);
  server.stop();
  return SortedReadyState{
      .requestReachedATick = status == std::future_status::ready,
      .readyStateReachedATick = readyStateSeen};
}

}  // namespace

/**
 * Given a server opened on port 0
 * When its address is asked
 * Then it names the port the system picked, not 0
 */
TEST(ServerApplication, ListensOnThePortTheSystemPicked) {
  ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};
  server.open();

  EXPECT_NE(server.localEndpoint().port, ANY_PORT);
}

/**
 * Given a server running on its own thread
 * When a few ticks pass and nobody calls stop()
 * Then run() has not returned
 */
TEST(ServerApplication, KeepsRunningUntilStopped) {
  ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};
  server.open();
  std::promise<void> returned;
  const std::future<void> runReturned = returned.get_future();
  const std::jthread runner{[&server, &returned] {
    server.run();
    returned.set_value();
  }};

  const std::future_status beforeStop = runReturned.wait_for(
      rtype::engine::SIMULATION_TICK_DURATION * SERVER_RUN_TICKS);
  server.stop();

  EXPECT_EQ(beforeStop, std::future_status::timeout);
}

/**
 * Given a server running on its own thread for a few ticks
 * When stop() is called from the test thread
 * Then run() returns
 */
TEST(ServerApplication, RunReturnsWhenStoppedFromAnotherThread) {
  ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};
  server.open();
  std::promise<void> returned;
  const std::future<void> runReturned = returned.get_future();
  const std::jthread runner{[&server, &returned] {
    server.run();
    returned.set_value();
  }};
  std::this_thread::sleep_for(rtype::engine::SIMULATION_TICK_DURATION *
                              SERVER_RUN_TICKS);

  server.stop();

  EXPECT_EQ(runReturned.wait_for(THREAD_TIMEOUT), std::future_status::ready);
}

/**
 * Given a running server whose tick handler records the connection requests
 * When a client sends it a connection request
 * Then a tick hands the handler that request, with the client as its sender
 */
TEST(ServerApplication, HandsAConnectionRequestToTheTick) {
  std::promise<IncomingDatagram> firstRequest;
  std::future<IncomingDatagram> received = firstRequest.get_future();
  std::atomic<bool> delivered{false};
  ServerApplication server{
      loopbackOnPort(ANY_PORT),
      [&firstRequest,
       &delivered](const ServerApplication::ReceivedDatagrams& datagrams) {
        if (!datagrams.connectionRequests.empty() &&
            !delivered.exchange(true)) {
          firstRequest.set_value(datagrams.connectionRequests.front());
        }
      }};
  server.open();
  const std::jthread runner{[&server] { server.run(); }};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackOnPort(ANY_PORT)};

  client.send(loopbackOnPort(server.localEndpoint().port), requestDatagram());
  clientContext.run();
  const std::future_status status = received.wait_for(THREAD_TIMEOUT);
  server.stop();

  ASSERT_EQ(status, std::future_status::ready);
  EXPECT_EQ(
      received.get(),
      (IncomingDatagram{.sender = loopbackOnPort(client.localEndpoint().port),
                        .payload = requestDatagram()}));
}

/**
 * Given a server that admitted a client before it started
 * When that client sends a datagram that is not a connection request
 * Then a tick hands the handler that datagram among the players' datagrams
 */
TEST(ServerApplication, HandsAnAdmittedPlayersDatagramToTheTick) {
  std::promise<IncomingDatagram> firstDatagram;
  std::future<IncomingDatagram> received = firstDatagram.get_future();
  std::atomic<bool> delivered{false};
  ServerApplication server{
      loopbackOnPort(ANY_PORT),
      [&firstDatagram,
       &delivered](const ServerApplication::ReceivedDatagrams& datagrams) {
        if (!datagrams.players.empty() && !delivered.exchange(true)) {
          firstDatagram.set_value(datagrams.players.front());
        }
      }};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackOnPort(ANY_PORT)};
  server.admit(loopbackOnPort(client.localEndpoint().port), SESSION_TOKEN);
  server.open();
  const std::jthread runner{[&server] { server.run(); }};

  client.send(loopbackOnPort(server.localEndpoint().port), playerReady());
  clientContext.run();
  const std::future_status status = received.wait_for(THREAD_TIMEOUT);
  server.stop();

  ASSERT_EQ(status, std::future_status::ready);
  EXPECT_EQ(
      received.get(),
      (IncomingDatagram{.sender = loopbackOnPort(client.localEndpoint().port),
                        .payload = playerReady()}));
}

/**
 * Given a running server whose tick admits the sender of the first connection
 * request it is handed, as the connection acceptor does
 * When a client sends a request, then a ReadyState once it was admitted
 * Then a tick hands the handler the ReadyState among the players' datagrams
 */
TEST(ServerApplication, HandsTheTickTheDatagramOfAPlayerItAdmitted) {
  std::promise<void> admission;
  const std::future<void> admitted = admission.get_future();
  std::promise<IncomingDatagram> firstDatagram;
  std::future<IncomingDatagram> received = firstDatagram.get_future();
  std::atomic<bool> admissionDone{false};
  std::atomic<bool> delivered{false};
  ServerApplication* serverPointer = nullptr;
  ServerApplication server{
      loopbackOnPort(ANY_PORT),
      [&](const ServerApplication::ReceivedDatagrams& datagrams) {
        if (!datagrams.connectionRequests.empty() &&
            !admissionDone.exchange(true)) {
          serverPointer->admit(datagrams.connectionRequests.front().sender,
                               SESSION_TOKEN);
          admission.set_value();
        }
        if (!datagrams.players.empty() && !delivered.exchange(true)) {
          firstDatagram.set_value(datagrams.players.front());
        }
      }};
  serverPointer = &server;
  server.open();
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackOnPort(ANY_PORT)};
  client.startReceiving(
      []([[maybe_unused]] const Endpoint& sender,
         [[maybe_unused]] std::span<const std::byte> bytes) {});
  const Endpoint serverEndpoint = loopbackOnPort(server.localEndpoint().port);
  const std::jthread serverRunner{[&server] { server.run(); }};
  const std::jthread clientRunner{[&clientContext] { clientContext.run(); }};

  clientContext.post([&client, &serverEndpoint] {
    client.send(serverEndpoint, requestDatagram());
  });
  const std::future_status admissionStatus = admitted.wait_for(THREAD_TIMEOUT);
  clientContext.post([&client, &serverEndpoint] {
    client.send(serverEndpoint, playerReady());
  });
  const std::future_status status = received.wait_for(THREAD_TIMEOUT);
  server.stop();
  clientContext.stop();

  ASSERT_EQ(admissionStatus, std::future_status::ready);
  ASSERT_EQ(status, std::future_status::ready);
  EXPECT_EQ(
      received.get(),
      (IncomingDatagram{.sender = loopbackOnPort(client.localEndpoint().port),
                        .payload = playerReady()}));
}

/**
 * Given a running server and a client nobody admitted
 * When the client sends a ReadyState, which only a player sends, then a
 * connection request
 * Then the request reaches a tick, and no tick ever sees the ReadyState
 */
TEST(ServerApplication, DropsAStrangersDatagramBeforeTheTick) {
  const SortedReadyState sorted =
      sortReadyStateThenRequest([]([[maybe_unused]] ServerApplication& server,
                                   [[maybe_unused]] const Endpoint& client) {},
                                readyDatagram());

  ASSERT_TRUE(sorted.requestReachedATick);
  EXPECT_FALSE(sorted.readyStateReachedATick);
}

/**
 * Given a server that admitted a client, then forgot it, before it started
 * When the client sends a ReadyState, then a connection request
 * Then the request reaches a tick, and no tick ever sees the ReadyState
 */
TEST(ServerApplication, DropsTheDatagramOfAForgottenPlayerBeforeTheTick) {
  const SortedReadyState sorted = sortReadyStateThenRequest(
      [](ServerApplication& server, const Endpoint& client) {
        server.admit(client, SESSION_TOKEN);
        server.forget(client, SESSION_TOKEN);
      },
      playerReady());

  ASSERT_TRUE(sorted.requestReachedATick);
  EXPECT_FALSE(sorted.readyStateReachedATick);
}

/**
 * Given a server whose tick handler throws
 * When the server runs
 * Then run() stops both threads and throws what the handler threw
 */
TEST(ServerApplication, RethrowsWhatTheTickThrew) {
  ServerApplication server{
      loopbackOnPort(ANY_PORT),
      []([[maybe_unused]] const ServerApplication::ReceivedDatagrams&
             received) { throw TickFailure{}; }};
  server.open();

  EXPECT_THROW(server.run(), TickFailure);
}

/**
 * Given a running server whose tick handler queues one datagram for a client
 * When the client listens on its socket
 * Then the client receives that datagram's bytes from the server's address
 */
TEST(ServerApplication, SendsTheDatagramsTheTickQueued) {
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackOnPort(ANY_PORT)};
  const std::vector<std::byte> payload(CLIENT_PAYLOAD_SIZE,
                                       CLIENT_PAYLOAD_BYTE);
  std::promise<std::vector<std::byte>> arrived;
  std::future<std::vector<std::byte>> received = arrived.get_future();
  std::atomic<bool> delivered{false};
  client.startReceiving([&arrived, &delivered, &clientContext](
                            [[maybe_unused]] const Endpoint& sender,
                            std::span<const std::byte> bytes) {
    if (!delivered.exchange(true)) {
      arrived.set_value(std::vector<std::byte>(bytes.begin(), bytes.end()));
      clientContext.stop();
    }
  });
  std::atomic<bool> queued{false};
  ServerApplication* serverPointer = nullptr;
  ServerApplication server{
      loopbackOnPort(ANY_PORT),
      [&]([[maybe_unused]] const ServerApplication::ReceivedDatagrams&
              datagrams) {
        if (!queued.exchange(true)) {
          serverPointer->queueDatagram(rtype::network::OutgoingDatagram{
              .destination = loopbackOnPort(client.localEndpoint().port),
              .payload = payload});
        }
      }};
  serverPointer = &server;
  server.open();
  const std::jthread serverRunner{[&server] { server.run(); }};
  const std::jthread clientRunner{[&clientContext] { clientContext.run(); }};

  const std::future_status status = received.wait_for(THREAD_TIMEOUT);
  server.stop();
  clientContext.stop();

  ASSERT_EQ(status, std::future_status::ready);
  EXPECT_EQ(received.get(), payload);
}

/**
 * Given a server that was built and not opened
 * When its address is asked
 * Then a ServerNotOpenException is thrown: the socket is bound by open()
 */
TEST(ServerApplication, HasNoAddressBeforeItIsOpened) {
  const ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};

  EXPECT_THROW(static_cast<void>(server.localEndpoint()),
               rtype::server::ServerNotOpenException);
}

/**
 * Given a server that was built and not opened
 * When it runs
 * Then a ServerNotOpenException is thrown
 */
TEST(ServerApplication, RefusesToRunBeforeItIsOpened) {
  ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};

  EXPECT_THROW(server.run(), rtype::server::ServerNotOpenException);
}

/**
 * Given a server that is open
 * When it is opened again
 * Then a ServerAlreadyOpenException is thrown and the socket stays bound
 */
TEST(ServerApplication, RefusesToOpenTwice) {
  ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};
  server.open();
  const std::uint16_t port = server.localEndpoint().port;

  EXPECT_THROW(server.open(), rtype::server::ServerAlreadyOpenException);
  EXPECT_EQ(server.localEndpoint().port, port);
}
