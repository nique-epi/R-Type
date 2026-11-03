#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>
#include "CountingDatagramSocket.hpp"
#include "Endpoint.hpp"
#include "MessageType.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkException.hpp"
#include "RecordingDatagramSocket.hpp"
#include "SimulatedNetworkSocket.hpp"
#include "TrafficCounters.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficTestConstants.hpp"

using rtype::network::CountingDatagramSocket;
using rtype::network::DatagramTooLargeException;
using rtype::network::Endpoint;
using rtype::network::MAX_DATAGRAM_SIZE;
using rtype::network::MessageType;
using rtype::network::NetworkConditions;
using rtype::network::NetworkContext;
using rtype::network::SimulatedNetworkSocket;
using rtype::network::TrafficCounters;
using rtype::network::testing::datagramHolding;
using rtype::network::testing::peerOnPort;

/**
 * Given a counting socket
 * When a datagram is sent
 * Then the wrapped socket sends it, and it is counted once
 */
TEST(CountingDatagramSocket, SendsThenCounts) {
  TrafficCounters counters;
  auto recording = std::make_unique<RecordingDatagramSocket>();
  const RecordingDatagramSocket& real = *recording;
  CountingDatagramSocket socket{std::move(recording), counters};

  socket.send(peerOnPort(PEER_PORT),
              datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD));

  EXPECT_EQ(real.sent().size(), 1U);
  EXPECT_EQ(counters.report().others.sent.datagrams.count, 1U);
}

/**
 * Given a counting socket
 * When a datagram larger than the largest is sent
 * Then the wrapped socket refuses it, and nothing is counted
 */
TEST(CountingDatagramSocket, ADatagramRefusedIsNotCounted) {
  TrafficCounters counters;
  CountingDatagramSocket socket{std::make_unique<RecordingDatagramSocket>(),
                                counters};

  EXPECT_THROW(socket.send(peerOnPort(PEER_PORT),
                           std::vector<std::byte>(MAX_DATAGRAM_SIZE + 1)),
               DatagramTooLargeException);

  EXPECT_EQ(counters.report().others.sent.datagrams.count, 0U);
}

/**
 * Given a counting socket that was handed a handler
 * When the wrapped socket receives a datagram
 * Then the handler gets it with its sender, and it is already counted
 */
TEST(CountingDatagramSocket, CountsThenHandsOnWhatItReceives) {
  TrafficCounters counters;
  auto recording = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& real = *recording;
  CountingDatagramSocket socket{std::move(recording), counters};
  std::size_t countedWhenHandled = 0;
  Endpoint handledSender;
  socket.startReceiving(
      [&](const Endpoint& sender,
          [[maybe_unused]] std::span<const std::byte> payload) {
        handledSender = sender;
        countedWhenHandled = counters.report().others.received.datagrams.count;
      });

  real.receive(peerOnPort(PEER_PORT),
               datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD));

  EXPECT_EQ(handledSender, peerOnPort(PEER_PORT));
  EXPECT_EQ(countedWhenHandled, 1U);
}

/**
 * Given a counting socket
 * When its address is asked
 * Then it gives the address of the wrapped socket
 */
TEST(CountingDatagramSocket, GivesTheAddressOfTheWrappedSocket) {
  TrafficCounters counters;
  auto recording = std::make_unique<RecordingDatagramSocket>();
  const RecordingDatagramSocket& real = *recording;
  const CountingDatagramSocket socket{std::move(recording), counters};

  EXPECT_EQ(socket.localEndpoint(), real.localEndpoint());
}

/**
 * Given a counting socket over a simulated network that loses everything
 * When a datagram is sent
 * Then it counts as sent, as on a real network, and never reached the socket
 */
TEST(CountingDatagramSocket, ADatagramLostOnTheWayOutCountsAsSent) {
  NetworkContext context;
  TrafficCounters counters;
  auto recording = std::make_unique<RecordingDatagramSocket>();
  const RecordingDatagramSocket& real = *recording;
  CountingDatagramSocket socket{
      std::make_unique<SimulatedNetworkSocket>(
          context, std::move(recording), NetworkConditions{.lossPercent = 100}),
      counters};

  socket.send(peerOnPort(PEER_PORT),
              datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD));
  context.run();

  EXPECT_EQ(counters.report().others.sent.datagrams.count, 1U);
  EXPECT_TRUE(real.sent().empty());
}

/**
 * Given a counting socket over a simulated network that loses everything
 * When the real socket receives a datagram
 * Then it does not count as received: the program never got it
 */
TEST(CountingDatagramSocket, ADatagramLostOnTheWayInIsNotCounted) {
  NetworkContext context;
  TrafficCounters counters;
  auto recording = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& real = *recording;
  CountingDatagramSocket socket{
      std::make_unique<SimulatedNetworkSocket>(
          context, std::move(recording), NetworkConditions{.lossPercent = 100}),
      counters};
  socket.startReceiving(
      []([[maybe_unused]] const Endpoint& sender,
         [[maybe_unused]] std::span<const std::byte> payload) {});

  real.receive(peerOnPort(PEER_PORT),
               datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD));
  context.run();

  EXPECT_EQ(counters.report().others.received.datagrams.count, 0U);
}
