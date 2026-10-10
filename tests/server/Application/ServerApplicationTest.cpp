#include <gtest/gtest.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <future>
#include <span>
#include <string>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkContext.hpp"
#include "NetworkTestConstants.hpp"
#include "ServerApplication.hpp"
#include "ServerTestConstants.hpp"
#include "TimeConstants.hpp"
#include "UdpSocket.hpp"

using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::server::ServerApplication;

namespace {

/**
 * @brief What a test tick handler throws to make the server stop.
 */
class TickFailure final : public std::exception {};

Endpoint loopbackOnPort(std::uint16_t port) {
  return Endpoint{.address = std::string{LOOPBACK_ADDRESS}, .port = port};
}

ServerApplication::TickHandler ignoringDatagrams() {
  return []([[maybe_unused]] std::span<const IncomingDatagram> datagrams) {};
}

}  // namespace

/**
 * Given a server opened on port 0
 * When its address is asked
 * Then it names the port the system picked, not 0
 */
TEST(ServerApplication, ListensOnThePortTheSystemPicked) {
  const ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};

  EXPECT_NE(server.localEndpoint().port, ANY_PORT);
}

/**
 * Given a server running on its own thread
 * When a few ticks pass and nobody calls stop()
 * Then run() has not returned
 */
TEST(ServerApplication, KeepsRunningUntilStopped) {
  ServerApplication server{loopbackOnPort(ANY_PORT), ignoringDatagrams()};
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
 * Given a running server whose tick handler records what it is given
 * When a client sends it a datagram
 * Then a tick hands the handler that datagram, with the client as its sender
 */
TEST(ServerApplication, HandsEachReceivedDatagramToTheTick) {
  std::promise<IncomingDatagram> firstDatagram;
  std::future<IncomingDatagram> received = firstDatagram.get_future();
  std::atomic<bool> delivered{false};
  ServerApplication server{
      loopbackOnPort(ANY_PORT),
      [&firstDatagram,
       &delivered](std::span<const IncomingDatagram> datagrams) {
        if (!datagrams.empty() && !delivered.exchange(true)) {
          firstDatagram.set_value(datagrams.front());
        }
      }};
  const std::jthread runner{[&server] { server.run(); }};
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackOnPort(ANY_PORT)};
  const std::vector<std::byte> payload(RELAY_PAYLOAD_SIZE, RECEIVED_BYTE);

  client.send(loopbackOnPort(server.localEndpoint().port), payload);
  clientContext.run();
  const std::future_status status = received.wait_for(THREAD_TIMEOUT);
  server.stop();

  ASSERT_EQ(status, std::future_status::ready);
  EXPECT_EQ(
      received.get(),
      (IncomingDatagram{.sender = loopbackOnPort(client.localEndpoint().port),
                        .payload = payload}));
}

/**
 * Given a server whose tick handler throws
 * When the server runs
 * Then run() stops both threads and throws what the handler threw
 */
TEST(ServerApplication, RethrowsWhatTheTickThrew) {
  ServerApplication server{
      loopbackOnPort(ANY_PORT),
      []([[maybe_unused]] std::span<const IncomingDatagram> datagrams) {
        throw TickFailure{};
      }};

  EXPECT_THROW(server.run(), TickFailure);
}
