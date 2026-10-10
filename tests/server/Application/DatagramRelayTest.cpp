#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramRelay.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingDatagramSocket.hpp"
#include "ServerTestConstants.hpp"

using rtype::engine::BoundedQueue;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::OutgoingDatagram;
using rtype::server::DatagramRelay;

namespace {

/**
 * @brief A relay between a recording socket and two queues, all owned
 * together.
 */
struct RelayUnderTest {
  RecordingDatagramSocket socket;
  BoundedQueue<IncomingDatagram> incoming{RELAY_QUEUE_CAPACITY};
  BoundedQueue<OutgoingDatagram> outgoing{RELAY_QUEUE_CAPACITY};
  DatagramRelay relay{socket, incoming, outgoing};
};

Endpoint peerOnPort(std::uint16_t port) {
  return Endpoint{.address = std::string{PEER_ADDRESS}, .port = port};
}

std::vector<std::byte> receivedBytes() {
  std::vector<std::byte> bytes(RELAY_PAYLOAD_SIZE, RECEIVED_BYTE);
  return bytes;
}

}  // namespace

/**
 * Given a relay that started receiving
 * When the socket receives a datagram
 * Then the incoming queue holds it, with its sender and its bytes
 */
TEST(DatagramRelay, CopiesEachReceivedDatagramIntoTheIncomingQueue) {
  RelayUnderTest setup;
  setup.relay.startReceiving();
  std::vector<IncomingDatagram> received;

  setup.socket.receive(peerOnPort(PEER_PORT), receivedBytes());
  setup.incoming.drainInto(received);

  EXPECT_EQ(received,
            (std::vector<IncomingDatagram>{IncomingDatagram{
                .sender = peerOnPort(PEER_PORT), .payload = receivedBytes()}}));
}

/**
 * Given a relay that copied a received datagram into the incoming queue
 * When the socket reuses its receive buffer for other bytes
 * Then the queued datagram still holds the bytes received
 */
TEST(DatagramRelay, KeepsTheBytesOnceTheReceiveBufferIsReused) {
  RelayUnderTest setup;
  setup.relay.startReceiving();
  std::vector<std::byte> receiveBuffer = receivedBytes();
  setup.socket.receive(peerOnPort(PEER_PORT), receiveBuffer);
  std::vector<IncomingDatagram> received;

  std::ranges::fill(receiveBuffer, OVERWRITTEN_BYTE);
  setup.incoming.drainInto(received);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(received.front().payload, receivedBytes());
}

/**
 * Given two datagrams waiting in the outgoing queue, for two peers
 * When the relay sends what is waiting
 * Then the socket sent both, in the order they were queued, each to its peer
 */
TEST(DatagramRelay, SendsEveryWaitingDatagramInTheOrderQueued) {
  RelayUnderTest setup;
  const OutgoingDatagram first{.destination = peerOnPort(PEER_PORT),
                               .payload = receivedBytes()};
  const OutgoingDatagram second{.destination = peerOnPort(OTHER_PEER_PORT),
                                .payload = receivedBytes()};
  setup.outgoing.push(first);
  setup.outgoing.push(second);

  setup.relay.sendWaiting();

  EXPECT_EQ(setup.socket.sent(),
            (std::vector<OutgoingDatagram>{first, second}));
}

/**
 * Given an empty outgoing queue
 * When the relay sends what is waiting
 * Then the socket sent nothing
 */
TEST(DatagramRelay, SendsNothingWhenNothingWaits) {
  RelayUnderTest setup;

  setup.relay.sendWaiting();

  EXPECT_TRUE(setup.socket.sent().empty());
}

/**
 * Given a datagram the relay already sent
 * When the relay sends what is waiting again
 * Then the datagram was sent once only
 */
TEST(DatagramRelay, NeverSendsADatagramTwice) {
  RelayUnderTest setup;
  const OutgoingDatagram datagram{.destination = peerOnPort(PEER_PORT),
                                  .payload = receivedBytes()};
  setup.outgoing.push(datagram);
  setup.relay.sendWaiting();

  setup.relay.sendWaiting();

  EXPECT_EQ(setup.socket.sent(), (std::vector<OutgoingDatagram>{datagram}));
}
