#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <future>
#include <span>
#include <string>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkException.hpp"
#include "NetworkTestConstants.hpp"
#include "NetworkThread.hpp"
#include "NetworkThreadTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "UdpSocket.hpp"

using rtype::client::NetworkThread;
using rtype::network::ANY_PORT;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::LOOPBACK_IPV4_ADDRESS;
using rtype::network::NetworkContext;
using rtype::network::OutgoingDatagram;
using rtype::network::UdpSocket;

namespace {

Endpoint loopbackOnPort(std::uint16_t port) {
  return Endpoint{.address = std::string{LOOPBACK_IPV4_ADDRESS}, .port = port};
}

std::vector<std::byte> exchangedBytes() {
  return {EXCHANGED_BYTES.begin(), EXCHANGED_BYTES.end()};
}

/**
 * @brief Drains @p networkThread the way the frame loop does, until something
 * arrived or DATAGRAM_TIMEOUT passed.
 * @return what arrived; nothing after DATAGRAM_TIMEOUT.
 */
std::vector<IncomingDatagram> drainUntilReceived(NetworkThread& networkThread) {
  const auto deadline = std::chrono::steady_clock::now() + DATAGRAM_TIMEOUT;
  std::vector<IncomingDatagram> received;
  networkThread.drainReceived(received);
  while (received.empty() && std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(DRAIN_INTERVAL);
    networkThread.drainReceived(received);
  }
  return received;
}

}  // namespace

/**
 * Given a network thread on the loopback address
 * When a peer sends it a datagram
 * Then draining it on the test thread returns that datagram, with the peer as
 * its sender
 */
TEST(NetworkThread, HandsEachReceivedDatagramToTheDrainingThread) {
  NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
  NetworkContext peerContext;
  UdpSocket peer{peerContext, loopbackOnPort(ANY_PORT)};

  peer.send(networkThread.localEndpoint(), exchangedBytes());
  peerContext.run();
  const std::vector<IncomingDatagram> received =
      drainUntilReceived(networkThread);

  EXPECT_EQ(received, (std::vector<IncomingDatagram>{IncomingDatagram{
                          .sender = loopbackOnPort(peer.localEndpoint().port),
                          .payload = exchangedBytes()}}));
}

/**
 * Given a network thread and a peer listening on the loopback address
 * When the test thread hands the network thread a datagram for the peer
 * Then the peer receives its bytes, sent from the network thread's port
 */
TEST(NetworkThread, SendsEachHandedDatagramToItsDestination) {
  NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
  NetworkContext peerContext;
  UdpSocket peer{peerContext, loopbackOnPort(ANY_PORT)};
  const Endpoint peerEndpoint = peer.localEndpoint();
  std::promise<IncomingDatagram> arrival;
  std::future<IncomingDatagram> arrived = arrival.get_future();
  peer.startReceiving(
      [&arrival, &peerContext](const Endpoint& sender,
                               std::span<const std::byte> payload) {
        arrival.set_value(IncomingDatagram{
            .sender = sender, .payload = {payload.begin(), payload.end()}});
        peerContext.stop();
      });
  const std::jthread peerThread{[&peerContext] { peerContext.run(); }};

  networkThread.send(OutgoingDatagram{.destination = peerEndpoint,
                                      .payload = exchangedBytes()});
  const std::future_status status = arrived.wait_for(DATAGRAM_TIMEOUT);
  peerContext.stop();

  ASSERT_EQ(status, std::future_status::ready);
  EXPECT_EQ(arrived.get(),
            (IncomingDatagram{.sender = networkThread.localEndpoint(),
                              .payload = exchangedBytes()}));
}

/**
 * Given a network thread that received nothing
 * When the test thread drains it
 * Then the call returns without adding anything
 */
TEST(NetworkThread, DrainingReturnsAtOnceWhenNothingArrived) {
  NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
  std::vector<IncomingDatagram> received;

  networkThread.drainReceived(received);

  EXPECT_TRUE(received.empty());
}

/**
 * Given a network thread
 * When it is handed a datagram larger than MAX_DATAGRAM_SIZE
 * Then the network thread stops, and draining it throws
 * DatagramTooLargeException
 */
TEST(NetworkThread, DrainingThrowsWhatStoppedTheNetworkThread) {
  NetworkThread networkThread{loopbackOnPort(ANY_PORT)};

  networkThread.send(OutgoingDatagram{
      .destination = networkThread.localEndpoint(),
      .payload = std::vector<std::byte>(OVERSIZED_PAYLOAD_SIZE)});

  EXPECT_THROW(drainUntilReceived(networkThread),
               rtype::network::DatagramTooLargeException);
}

/**
 * Given network threads destroyed as soon as they are created, some before
 * their thread ran the network
 * When each one is destroyed
 * Then its destructor returns, instead of waiting for a network that was told
 * to stop before it started
 */
TEST(NetworkThread, StopsEvenWhenDestroyedBeforeItsThreadRuns) {
  for (int created = 0; created < IMMEDIATE_DESTRUCTION_COUNT; ++created) {
    const NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
  }

  SUCCEED();
}
