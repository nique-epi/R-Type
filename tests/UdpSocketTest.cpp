#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "NetworkTestConstants.hpp"
#include "UdpSocket.hpp"
#include "UdpSocketFixture.hpp"

using rtype::network::DatagramTooLargeException;
using rtype::network::Endpoint;
using rtype::network::InvalidAddressException;
using rtype::network::MAX_DATAGRAM_SIZE;
using rtype::network::ReceivingAlreadyStartedException;
using rtype::network::SocketOpenException;
using rtype::network::UdpSocket;

/**
 * Given a server socket that sends every datagram back to its sender
 * When a client sends it a datagram
 * Then the client receives the same bytes back
 */
TEST_F(UdpSocketFixture, EchoesADatagramBackToItsSender) {
  UdpSocket server{context(), loopbackOnPort(ANY_PORT)};
  server.startReceiving(
      [&server](const Endpoint& sender, std::span<const std::byte> payload) {
        server.send(sender, payload);
      });
  auto client = openClient();
  const std::vector<std::byte> payload = makePayload(SMALL_PAYLOAD_SIZE);

  sendFromClient(client, server.localEndpoint().port, payload);

  EXPECT_EQ(receiveOnClient(client), payload);
}

/**
 * Given a server socket that sends every datagram back to its sender
 * When a client sends it a datagram of exactly MAX_DATAGRAM_SIZE bytes
 * Then the client receives all of them back
 */
TEST_F(UdpSocketFixture, EchoesADatagramOfTheMaximumSize) {
  UdpSocket server{context(), loopbackOnPort(ANY_PORT)};
  server.startReceiving(
      [&server](const Endpoint& sender, std::span<const std::byte> payload) {
        server.send(sender, payload);
      });
  auto client = openClient();
  const std::vector<std::byte> payload = makePayload(MAX_DATAGRAM_SIZE);

  sendFromClient(client, server.localEndpoint().port, payload);

  EXPECT_EQ(receiveOnClient(client), payload);
}

/**
 * Given a server socket receiving datagrams
 * When a client sends it a datagram
 * Then the handler gets the address and the port the client sent it from
 */
TEST_F(UdpSocketFixture, ReportsTheAddressAndPortOfTheSender) {
  UdpSocket server{context(), loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient();

  sendFromClient(client, server.localEndpoint().port,
                 makePayload(SMALL_PAYLOAD_SIZE));

  ASSERT_TRUE(runUntil([&received] { return !received.empty(); }));
  EXPECT_EQ(received.front().sender,
            loopbackOnPort(client.local_endpoint().port()));
}

/**
 * Given a server socket receiving datagrams
 * When a client sends it a datagram that holds no byte
 * Then the handler gets an empty payload
 */
TEST_F(UdpSocketFixture, DeliversAnEmptyDatagram) {
  UdpSocket server{context(), loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient();

  sendFromClient(client, server.localEndpoint().port, {});

  ASSERT_TRUE(runUntil([&received] { return !received.empty(); }));
  EXPECT_EQ(received.front().payload, std::vector<std::byte>{});
}

/**
 * Given a server socket receiving datagrams
 * When a client sends it a datagram one byte over MAX_DATAGRAM_SIZE, then a
 *      small one
 * Then the handler only gets the small one
 */
TEST_F(UdpSocketFixture, DropsADatagramOneByteOverTheMaximumSize) {
  UdpSocket server{context(), loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient();

  sendFromClient(client, server.localEndpoint().port,
                 makePayload(MAX_DATAGRAM_SIZE + 1));
  sendFromClient(client, server.localEndpoint().port,
                 makePayload(SMALL_PAYLOAD_SIZE));

  ASSERT_TRUE(runUntil([&received] { return !received.empty(); }));
  EXPECT_EQ(received.front().payload, makePayload(SMALL_PAYLOAD_SIZE));
}

/**
 * Given a server socket receiving datagrams
 * When a client sends it a datagram larger than its receive buffer, then a
 *      small one
 * Then the handler only gets the small one
 */
TEST_F(UdpSocketFixture, DropsADatagramLargerThanItsReceiveBuffer) {
  UdpSocket server{context(), loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient();

  sendFromClient(client, server.localEndpoint().port,
                 makePayload(OVERSIZED_PAYLOAD_SIZE));
  sendFromClient(client, server.localEndpoint().port,
                 makePayload(SMALL_PAYLOAD_SIZE));

  ASSERT_TRUE(runUntil([&received] { return !received.empty(); }));
  EXPECT_EQ(received.front().payload, makePayload(SMALL_PAYLOAD_SIZE));
}

/**
 * Given a server socket that sent a datagram to a port nobody listens on
 * When a client then sends the server a datagram
 * Then the handler still gets it
 */
TEST_F(UdpSocketFixture, KeepsReceivingAfterSendingToAClosedPort) {
  UdpSocket server{context(), loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient();
  auto closedSocket = openClient();
  const std::uint16_t closedPort = closedSocket.local_endpoint().port();
  closedSocket.close();
  server.send(loopbackOnPort(closedPort), makePayload(SMALL_PAYLOAD_SIZE));

  sendFromClient(client, server.localEndpoint().port,
                 makePayload(SMALL_PAYLOAD_SIZE));

  EXPECT_TRUE(runUntil([&received] { return !received.empty(); }));
}

/**
 * Given a datagram already delivered to a server socket that started receiving
 * When the socket is destroyed before its event loop runs
 * Then running the loop never hands the datagram to the handler
 */
TEST_F(UdpSocketFixture, NeverCallsTheHandlerOnceDestroyed) {
  auto client = openClient();
  std::vector<ReceivedDatagram> received;
  {
    UdpSocket server{context(), loopbackOnPort(ANY_PORT)};
    sendFromClient(client, server.localEndpoint().port,
                   makePayload(SMALL_PAYLOAD_SIZE));
    std::this_thread::sleep_for(LOOPBACK_DELIVERY_TIME);
    server.startReceiving(recordInto(received));
  }

  runUntilIdle();

  EXPECT_EQ(received.size(), 0U);
}

/**
 * Given a socket that already hands its datagrams to a handler
 * When it is asked to start receiving again
 * Then it throws ReceivingAlreadyStartedException
 */
TEST_F(UdpSocketFixture, RefusesToStartReceivingTwice) {
  UdpSocket socket{context(), loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  socket.startReceiving(recordInto(received));

  EXPECT_THROW(socket.startReceiving(recordInto(received)),
               ReceivingAlreadyStartedException);
}

/**
 * Given an open socket
 * When it is asked to send one byte more than MAX_DATAGRAM_SIZE
 * Then it throws DatagramTooLargeException
 */
TEST_F(UdpSocketFixture, RefusesToSendMoreThanTheMaximumSize) {
  UdpSocket socket{context(), loopbackOnPort(ANY_PORT)};

  EXPECT_THROW(
      socket.send(socket.localEndpoint(), makePayload(MAX_DATAGRAM_SIZE + 1)),
      DatagramTooLargeException);
}

/**
 * Given an open socket
 * When it is asked to send to a host name instead of an IP address
 * Then it throws InvalidAddressException
 */
TEST_F(UdpSocketFixture, RefusesToSendToAHostName) {
  UdpSocket socket{context(), loopbackOnPort(ANY_PORT)};

  EXPECT_THROW(socket.send(Endpoint{.address = std::string{HOST_NAME},
                                    .port = socket.localEndpoint().port},
                           makePayload(SMALL_PAYLOAD_SIZE)),
               InvalidAddressException);
}

/**
 * Given a network context
 * When a socket is opened on a host name instead of an IP address
 * Then it throws InvalidAddressException
 */
TEST_F(UdpSocketFixture, RefusesToOpenOnAHostName) {
  EXPECT_THROW(openSocketOn(Endpoint{.address = std::string{HOST_NAME},
                                     .port = ANY_PORT}),
               InvalidAddressException);
}

/**
 * Given a socket bound to a port
 * When a second socket is opened on the same address and port
 * Then it throws SocketOpenException
 */
TEST_F(UdpSocketFixture, RefusesToOpenOnAPortAlreadyTaken) {
  const UdpSocket first{context(), loopbackOnPort(ANY_PORT)};

  EXPECT_THROW(openSocketOn(first.localEndpoint()), SocketOpenException);
}
