#include <gtest/gtest.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
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
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerApplication.hpp"
#include "ServerApplicationFixtures.hpp"
#include "ServerDatagrams.hpp"
#include "ServerException.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "TimeConstants.hpp"
#include "UdpSocket.hpp"

using rtype::network::ANY_PORT;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::server::ServerApplication;
using rtype::server::testing::SESSION_TOKEN;

/**
 * Given a server built on port 0
 * When it opens
 * Then the address it gives names the port the system picked, not 0
 */
TEST(ServerApplication, ListensOnThePortTheSystemPicked) {
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks()};

  const Endpoint bound = server.open();

  EXPECT_NE(bound.port, ANY_PORT);
}

/**
 * Given a server running on its own thread
 * When a few ticks pass and nobody calls stop()
 * Then run() has not returned
 */
TEST(ServerApplication, KeepsRunningUntilStopped) {
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks()};
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
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks()};
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
      loopbackEndpoint(ANY_PORT),
      [&firstRequest,
       &delivered](const ServerApplication::ReceivedDatagrams& datagrams) {
        if (!datagrams.connectionRequests.empty() &&
            !delivered.exchange(true)) {
          firstRequest.set_value(datagrams.connectionRequests.front());
        }
      }};
  const Endpoint bound = server.open();
  const std::jthread runner{[&server] { server.run(); }};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackEndpoint(ANY_PORT)};

  client.send(loopbackEndpoint(bound.port), requestDatagram());
  clientContext.run();
  const std::future_status status = received.wait_for(THREAD_TIMEOUT);
  server.stop();

  ASSERT_EQ(status, std::future_status::ready);
  const IncomingDatagram request = received.get();
  EXPECT_EQ(request.sender, loopbackEndpoint(client.localEndpoint().port));
  EXPECT_EQ(request.payload, requestDatagram());
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
      loopbackEndpoint(ANY_PORT),
      [&firstDatagram,
       &delivered](const ServerApplication::ReceivedDatagrams& datagrams) {
        if (!datagrams.players.empty() && !delivered.exchange(true)) {
          firstDatagram.set_value(datagrams.players.front());
        }
      }};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackEndpoint(ANY_PORT)};
  server.admit(loopbackEndpoint(client.localEndpoint().port), SESSION_TOKEN);
  const Endpoint bound = server.open();
  const std::jthread runner{[&server] { server.run(); }};

  client.send(loopbackEndpoint(bound.port), playerReady());
  clientContext.run();
  const std::future_status status = received.wait_for(THREAD_TIMEOUT);
  server.stop();

  ASSERT_EQ(status, std::future_status::ready);
  const IncomingDatagram datagram = received.get();
  EXPECT_EQ(datagram.sender, loopbackEndpoint(client.localEndpoint().port));
  EXPECT_EQ(datagram.payload, playerReady());
}

/**
 * Given a running server whose tick admits the sender of the first connection
 * request it is handed, as the connection acceptor does
 * When a client sends a request, then a ReadyState once it was admitted
 * Then a tick hands the handler the ReadyState among the players' datagrams
 */
TEST(ServerApplication, HandsTheTickTheDatagramOfAPlayerItAdmitted) {
  AdmissionWatch watch;
  const std::future<void> admitted = watch.admission.get_future();
  std::future<IncomingDatagram> received = watch.firstDatagram.get_future();
  ServerApplication server{loopbackEndpoint(ANY_PORT),
                           admittingFirstRequester(watch)};
  watch.server = &server;
  const Endpoint bound = server.open();
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackEndpoint(ANY_PORT)};
  client.startReceiving(ignoringWhatItReceives());
  const Endpoint serverEndpoint = loopbackEndpoint(bound.port);
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
  const IncomingDatagram datagram = received.get();
  EXPECT_EQ(datagram.sender, loopbackEndpoint(client.localEndpoint().port));
  EXPECT_EQ(datagram.payload, playerReady());
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
      loopbackEndpoint(ANY_PORT),
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
  rtype::network::UdpSocket client{clientContext, loopbackEndpoint(ANY_PORT)};
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
  QueuedOnce once{
      .datagram = {.destination = loopbackEndpoint(client.localEndpoint().port),
                   .payload = payload}};
  ServerApplication server{loopbackEndpoint(ANY_PORT), queueingOnce(once)};
  once.server = &server;
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
 * When it runs
 * Then a ServerNotOpenException is thrown
 */
TEST(ServerApplication, RefusesToRunBeforeItIsOpened) {
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks()};

  EXPECT_THROW(server.run(), rtype::server::ServerNotOpenException);
}

/**
 * Given a server that is open
 * When it is opened again
 * Then a ServerAlreadyOpenException is thrown and the socket stays bound
 */
TEST(ServerApplication, RefusesToOpenTwice) {
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks()};
  const std::uint16_t port = server.open().port;

  EXPECT_THROW(server.open(), rtype::server::ServerAlreadyOpenException);
  rtype::network::NetworkContext otherContext;
  EXPECT_THROW(rtype::network::UdpSocket(otherContext, loopbackEndpoint(port)),
               rtype::network::SocketOpenException);
}

/**
 * Given a server that is open on a port the system picked
 * When it is closed
 * Then another socket can bind that port, and the server can open again
 */
TEST(ServerApplication, GivesItsPortBackWhenClosed) {
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks()};
  const std::uint16_t port = server.open().port;

  server.close();

  rtype::network::NetworkContext otherContext;
  EXPECT_NO_THROW(
      rtype::network::UdpSocket(otherContext, loopbackEndpoint(port)));
  EXPECT_NO_THROW(server.open());
}
