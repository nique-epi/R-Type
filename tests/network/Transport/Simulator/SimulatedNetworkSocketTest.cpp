#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "NetworkSimulatorTestConstants.hpp"
#include "NetworkTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "SimulatedNetworkSocket.hpp"
#include "SimulatedNetworkSocketFixture.hpp"
#include "SimulatedNetworkSocketTestConstants.hpp"
#include "SimulatorFixtures.hpp"
#include "SimulatorTestConstants.hpp"
#include "UdpSocket.hpp"

using rtype::network::DatagramTooLargeException;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::InvalidAddressException;
using rtype::network::MAX_DATAGRAM_SIZE;
using rtype::network::OutgoingDatagram;
using rtype::network::ReceivingAlreadyStartedException;
using rtype::network::SimulatedNetworkSocket;
using rtype::network::UdpSocket;

/**
 * Given a simulated socket with a lag of 200 ms
 * When it is handed a datagram and its event loop runs for 50 ms
 * Then the real socket was handed nothing yet
 */
TEST_F(SimulatedNetworkSocketFixture, SendsNothingBeforeHalfTheLagPassed) {
  const auto socket = overRecording(LAGGING_NETWORK);

  socket->send(peer(), CROSSING_BYTES);
  runAtMostOneHandlerFor(LESS_THAN_HALF_OF_THE_LAG);

  EXPECT_TRUE(recording().sent().empty());
}

/**
 * Given a simulated socket with a lag of 200 ms
 * When it is handed a datagram and its event loop runs a handler
 * Then the real socket sends that datagram, no sooner than 100 ms after
 */
TEST_F(SimulatedNetworkSocketFixture, SendsADatagramOnceHalfTheLagPassed) {
  const auto socket = overRecording(LAGGING_NETWORK);
  const auto handedAt = std::chrono::steady_clock::now();

  socket->send(peer(), CROSSING_BYTES);
  const bool ran = runOneHandler();
  const auto waited = std::chrono::steady_clock::now() - handedAt;

  ASSERT_TRUE(ran);
  EXPECT_EQ(recording().sent(),
            (std::vector<OutgoingDatagram>{OutgoingDatagram{
                .destination = peer(), .payload = crossingBytes()}}));
  EXPECT_GE(waited, HALF_OF_THE_LAG);
}

/**
 * Given a simulated socket with a lag of 200 ms
 * When it is handed a datagram whose bytes the caller then overwrites
 * Then the real socket sends the bytes as they were handed over
 */
TEST_F(SimulatedNetworkSocketFixture, SendsTheBytesAsTheyWereHandedOver) {
  const auto socket = overRecording(LAGGING_NETWORK);
  std::vector<std::byte> bytes = crossingBytes();

  socket->send(peer(), bytes);
  std::ranges::fill(bytes, OVERWRITING_BYTE);
  const bool ran = runOneHandler();

  ASSERT_TRUE(ran);
  ASSERT_EQ(recording().sent().size(), 1U);
  EXPECT_EQ(recording().sent().front().payload, crossingBytes());
}

/**
 * Given a simulated socket with a lag of 200 ms, receiving
 * When the real socket receives a datagram and the event loop runs for 50 ms
 * Then the handler got nothing yet
 */
TEST_F(SimulatedNetworkSocketFixture, ReceivesNothingBeforeHalfTheLagPassed) {
  const auto socket = overRecording(LAGGING_NETWORK);
  std::vector<IncomingDatagram> received;
  socket->startReceiving(recordInto(received));

  recording().receive(peer(), CROSSING_BYTES);
  runAtMostOneHandlerFor(LESS_THAN_HALF_OF_THE_LAG);

  EXPECT_TRUE(received.empty());
}

/**
 * Given a simulated socket with a lag of 200 ms, receiving
 * When the real socket receives a datagram and the event loop runs a handler
 * Then the handler gets that datagram, no sooner than 100 ms after
 */
TEST_F(SimulatedNetworkSocketFixture, ReceivesADatagramOnceHalfTheLagPassed) {
  const auto socket = overRecording(LAGGING_NETWORK);
  std::vector<IncomingDatagram> received;
  socket->startReceiving(recordInto(received));
  const auto receivedAt = std::chrono::steady_clock::now();

  recording().receive(peer(), CROSSING_BYTES);
  const bool ran = runOneHandler();
  const auto waited = std::chrono::steady_clock::now() - receivedAt;

  ASSERT_TRUE(ran);
  EXPECT_EQ(received, (std::vector<IncomingDatagram>{IncomingDatagram{
                          .sender = peer(), .payload = crossingBytes()}}));
  EXPECT_GE(waited, HALF_OF_THE_LAG);
}

/**
 * Given a simulated socket with a lag of 200 ms, receiving
 * When the real socket receives a datagram, then its buffer is overwritten
 * Then the handler gets the bytes as they were received
 */
TEST_F(SimulatedNetworkSocketFixture, ReceivesTheBytesAsTheyArrived) {
  const auto socket = overRecording(LAGGING_NETWORK);
  std::vector<IncomingDatagram> received;
  socket->startReceiving(recordInto(received));
  std::vector<std::byte> buffer = crossingBytes();

  recording().receive(peer(), buffer);
  std::ranges::fill(buffer, OVERWRITING_BYTE);
  const bool ran = runOneHandler();

  ASSERT_TRUE(ran);
  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(received.front().payload, crossingBytes());
}

/**
 * Given a simulated socket with a lag of 200 ms
 * When it is handed a datagram one byte larger than MAX_DATAGRAM_SIZE
 * Then it refuses it at once, as the real socket would
 */
TEST_F(SimulatedNetworkSocketFixture, RefusesADatagramTooLargeAtOnce) {
  const auto socket = overRecording(LAGGING_NETWORK);
  const std::vector<std::byte> tooLarge(MAX_DATAGRAM_SIZE + 1);

  EXPECT_THROW(socket->send(peer(), tooLarge), DatagramTooLargeException);
}

/**
 * Given a simulated socket with a lag of 200 ms
 * When it is handed a datagram for a host name, which is no IP address
 * Then it refuses it at once, as the real socket would
 */
TEST_F(SimulatedNetworkSocketFixture, RefusesADestinationThatIsNoIpAddress) {
  const auto socket = overRecording(LAGGING_NETWORK);
  const Endpoint named{.address = std::string{HOST_NAME}, .port = PEER_PORT};

  EXPECT_THROW(socket->send(named, CROSSING_BYTES), InvalidAddressException);
}

/**
 * Given a simulated socket with a lag of 200 ms that received a datagram
 * When the socket is destroyed before the datagram crossed, and the event
 * loop runs until it has no work left
 * Then the handler never gets the datagram
 */
TEST_F(SimulatedNetworkSocketFixture, DeliversNothingOnceDestroyed) {
  std::vector<IncomingDatagram> received;
  {
    const auto socket = overRecording(LAGGING_NETWORK);
    socket->startReceiving(recordInto(received));
    recording().receive(peer(), CROSSING_BYTES);
  }

  runUntilIdle();

  EXPECT_TRUE(received.empty());
}

/**
 * Given a simulated socket over a UDP socket bound on the loopback address
 * When its address is asked
 * Then it is the address and port of the UDP socket
 */
TEST_F(SimulatedNetworkSocketFixture, IsBoundWhereTheRealSocketIs) {
  auto udpSocket = std::make_unique<UdpSocket>(context(), loopbackOnAnyPort());
  const Endpoint bound = udpSocket->localEndpoint();

  const SimulatedNetworkSocket socket{context(), std::move(udpSocket),
                                      LAGGING_NETWORK};

  EXPECT_EQ(socket.localEndpoint(), bound);
}

/**
 * Given a simulated socket over a UDP socket, receiving
 * When it is asked to start receiving again
 * Then it refuses, as the real socket does
 */
TEST_F(SimulatedNetworkSocketFixture, StartsReceivingOnlyOnce) {
  SimulatedNetworkSocket socket{
      context(), std::make_unique<UdpSocket>(context(), loopbackOnAnyPort()),
      LAGGING_NETWORK};
  std::vector<IncomingDatagram> received;
  socket.startReceiving(recordInto(received));

  EXPECT_THROW(socket.startReceiving(recordInto(received)),
               ReceivingAlreadyStartedException);
}
