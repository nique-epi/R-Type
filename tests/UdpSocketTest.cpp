#include <gtest/gtest.h>
#include <array>
#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/udp.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <system_error>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkException.hpp"
#include "NetworkTestConstants.hpp"
#include "UdpSocket.hpp"

using rtype::network::DatagramHandler;
using rtype::network::DatagramTooLargeException;
using rtype::network::Endpoint;
using rtype::network::InvalidAddressException;
using rtype::network::MAX_DATAGRAM_SIZE;
using rtype::network::NetworkContext;
using rtype::network::ReceivingAlreadyStartedException;
using rtype::network::SocketOpenException;
using rtype::network::UdpSocket;

namespace {

struct ReceivedDatagram {
  Endpoint sender;
  std::vector<std::byte> payload;
};

Endpoint loopbackOnPort(std::uint16_t port) {
  return Endpoint{.address = std::string{LOOPBACK_ADDRESS}, .port = port};
}

/**
 * @return @p size bytes, each one different from the next, so that a lost,
 *         shifted or swapped byte shows.
 */
std::vector<std::byte> makePayload(std::size_t size) {
  std::vector<std::byte> payload(size);
  for (std::size_t i = 0; i < size; ++i) {
    payload[i] = static_cast<std::byte>(static_cast<unsigned char>(i));
  }
  return payload;
}

DatagramHandler recordInto(std::vector<ReceivedDatagram>& received) {
  return
      [&received](const Endpoint& sender, std::span<const std::byte> payload) {
        received.push_back(ReceivedDatagram{
            .sender = sender, .payload = {payload.begin(), payload.end()}});
      };
}

/**
 * @brief Runs @p context until @p isDone holds, for DATAGRAM_TIMEOUT at most.
 * @return whether @p isDone holds.
 */
bool runUntil(NetworkContext& context, const std::function<bool()>& isDone) {
  const auto deadline = std::chrono::steady_clock::now() + DATAGRAM_TIMEOUT;
  while (!isDone()) {
    if (context.ioContext().run_one_until(deadline) == 0) {
      return false;
    }
  }
  return true;
}

void openSocketOn(NetworkContext& context, const Endpoint& localEndpoint) {
  const UdpSocket socket{context, localEndpoint};
}

/**
 * @brief The test client: a plain Asio socket on the loopback address, so
 *        that UdpSocket is checked against something it does not share code
 *        with.
 */
asio::ip::udp::socket openClient(NetworkContext& context) {
  return asio::ip::udp::socket{
      context.ioContext(),
      asio::ip::udp::endpoint{
          asio::ip::make_address(std::string{LOOPBACK_ADDRESS}), ANY_PORT}};
}

void sendFromClient(asio::ip::udp::socket& client, std::uint16_t port,
                    std::span<const std::byte> payload) {
  client.send_to(
      asio::buffer(payload.data(), payload.size()),
      asio::ip::udp::endpoint{
          asio::ip::make_address(std::string{LOOPBACK_ADDRESS}), port});
}

/**
 * @return the next datagram the client receives, or nothing after
 *         DATAGRAM_TIMEOUT.
 */
std::optional<std::vector<std::byte>> receiveOnClient(
    asio::ip::udp::socket& client, NetworkContext& context) {
  std::array<std::byte, rtype::network::RECEIVE_BUFFER_SIZE> buffer{};
  asio::ip::udp::endpoint sender;
  std::optional<std::size_t> byteCount;
  client.async_receive_from(
      asio::buffer(buffer), sender,
      [&byteCount](const std::error_code& error, std::size_t count) {
        if (!error) {
          byteCount = count;
        }
      });
  runUntil(context, [&byteCount] { return byteCount.has_value(); });
  if (!byteCount.has_value()) {
    client.cancel();
    return std::nullopt;
  }
  const auto received = std::span<const std::byte>{buffer}.first(*byteCount);
  return std::vector<std::byte>{received.begin(), received.end()};
}

}  // namespace

/**
 * Given a server socket that sends every datagram back to its sender
 * When a client sends it a datagram
 * Then the client receives the same bytes back
 */
TEST(UdpSocket, EchoesADatagramBackToItsSender) {
  NetworkContext context;
  UdpSocket server{context, loopbackOnPort(ANY_PORT)};
  server.startReceiving(
      [&server](const Endpoint& sender, std::span<const std::byte> payload) {
        server.send(sender, payload);
      });
  auto client = openClient(context);
  const std::vector<std::byte> payload = makePayload(SMALL_PAYLOAD_SIZE);

  sendFromClient(client, server.localEndpoint().port, payload);

  EXPECT_EQ(receiveOnClient(client, context), payload);
}

/**
 * Given a server socket that sends every datagram back to its sender
 * When a client sends it a datagram of exactly MAX_DATAGRAM_SIZE bytes
 * Then the client receives all of them back
 */
TEST(UdpSocket, EchoesADatagramOfTheMaximumSize) {
  NetworkContext context;
  UdpSocket server{context, loopbackOnPort(ANY_PORT)};
  server.startReceiving(
      [&server](const Endpoint& sender, std::span<const std::byte> payload) {
        server.send(sender, payload);
      });
  auto client = openClient(context);
  const std::vector<std::byte> payload = makePayload(MAX_DATAGRAM_SIZE);

  sendFromClient(client, server.localEndpoint().port, payload);

  EXPECT_EQ(receiveOnClient(client, context), payload);
}

/**
 * Given a server socket receiving datagrams
 * When a client sends it a datagram
 * Then the handler gets the address and the port the client sent it from
 */
TEST(UdpSocket, ReportsTheAddressAndPortOfTheSender) {
  NetworkContext context;
  UdpSocket server{context, loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient(context);

  sendFromClient(client, server.localEndpoint().port,
                 makePayload(SMALL_PAYLOAD_SIZE));

  ASSERT_TRUE(runUntil(context, [&received] { return !received.empty(); }));
  EXPECT_EQ(received.front().sender,
            loopbackOnPort(client.local_endpoint().port()));
}

/**
 * Given a server socket receiving datagrams
 * When a client sends it a datagram that holds no byte
 * Then the handler gets an empty payload
 */
TEST(UdpSocket, DeliversAnEmptyDatagram) {
  NetworkContext context;
  UdpSocket server{context, loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient(context);

  sendFromClient(client, server.localEndpoint().port, {});

  ASSERT_TRUE(runUntil(context, [&received] { return !received.empty(); }));
  EXPECT_EQ(received.front().payload, std::vector<std::byte>{});
}

/**
 * Given a server socket receiving datagrams
 * When a client sends it a datagram one byte over MAX_DATAGRAM_SIZE, then a
 *      small one
 * Then the handler only gets the small one
 */
TEST(UdpSocket, DropsADatagramOneByteOverTheMaximumSize) {
  NetworkContext context;
  UdpSocket server{context, loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient(context);

  sendFromClient(client, server.localEndpoint().port,
                 makePayload(MAX_DATAGRAM_SIZE + 1));
  sendFromClient(client, server.localEndpoint().port,
                 makePayload(SMALL_PAYLOAD_SIZE));

  ASSERT_TRUE(runUntil(context, [&received] { return !received.empty(); }));
  EXPECT_EQ(received.front().payload, makePayload(SMALL_PAYLOAD_SIZE));
}

/**
 * Given a server socket receiving datagrams
 * When a client sends it a datagram larger than its receive buffer, then a
 *      small one
 * Then the handler only gets the small one
 */
TEST(UdpSocket, DropsADatagramLargerThanItsReceiveBuffer) {
  NetworkContext context;
  UdpSocket server{context, loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient(context);

  sendFromClient(client, server.localEndpoint().port,
                 makePayload(OVERSIZED_PAYLOAD_SIZE));
  sendFromClient(client, server.localEndpoint().port,
                 makePayload(SMALL_PAYLOAD_SIZE));

  ASSERT_TRUE(runUntil(context, [&received] { return !received.empty(); }));
  EXPECT_EQ(received.front().payload, makePayload(SMALL_PAYLOAD_SIZE));
}

/**
 * Given a server socket that sent a datagram to a port nobody listens on
 * When a client then sends the server a datagram
 * Then the handler still gets it
 */
TEST(UdpSocket, KeepsReceivingAfterSendingToAClosedPort) {
  NetworkContext context;
  UdpSocket server{context, loopbackOnPort(ANY_PORT)};
  std::vector<ReceivedDatagram> received;
  server.startReceiving(recordInto(received));
  auto client = openClient(context);
  auto closedSocket = openClient(context);
  const std::uint16_t closedPort = closedSocket.local_endpoint().port();
  closedSocket.close();
  server.send(loopbackOnPort(closedPort), makePayload(SMALL_PAYLOAD_SIZE));

  sendFromClient(client, server.localEndpoint().port,
                 makePayload(SMALL_PAYLOAD_SIZE));

  EXPECT_TRUE(runUntil(context, [&received] { return !received.empty(); }));
}

/**
 * Given a datagram already delivered to a server socket that started receiving
 * When the socket is destroyed before its event loop runs
 * Then running the loop never hands the datagram to the handler
 */
TEST(UdpSocket, NeverCallsTheHandlerOnceDestroyed) {
  NetworkContext context;
  auto client = openClient(context);
  std::vector<ReceivedDatagram> received;
  {
    UdpSocket server{context, loopbackOnPort(ANY_PORT)};
    sendFromClient(client, server.localEndpoint().port,
                   makePayload(SMALL_PAYLOAD_SIZE));
    std::this_thread::sleep_for(LOOPBACK_DELIVERY_TIME);
    server.startReceiving(recordInto(received));
  }

  context.ioContext().run_for(DATAGRAM_TIMEOUT);

  EXPECT_EQ(received.size(), 0U);
}

/**
 * Given a socket that already hands its datagrams to a handler
 * When it is asked to start receiving again
 * Then it throws ReceivingAlreadyStartedException
 */
TEST(UdpSocket, RefusesToStartReceivingTwice) {
  NetworkContext context;
  UdpSocket socket{context, loopbackOnPort(ANY_PORT)};
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
TEST(UdpSocket, RefusesToSendMoreThanTheMaximumSize) {
  NetworkContext context;
  UdpSocket socket{context, loopbackOnPort(ANY_PORT)};

  EXPECT_THROW(
      socket.send(socket.localEndpoint(), makePayload(MAX_DATAGRAM_SIZE + 1)),
      DatagramTooLargeException);
}

/**
 * Given an open socket
 * When it is asked to send to a host name instead of an IP address
 * Then it throws InvalidAddressException
 */
TEST(UdpSocket, RefusesToSendToAHostName) {
  NetworkContext context;
  UdpSocket socket{context, loopbackOnPort(ANY_PORT)};

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
TEST(UdpSocket, RefusesToOpenOnAHostName) {
  NetworkContext context;

  EXPECT_THROW(openSocketOn(context, Endpoint{.address = std::string{HOST_NAME},
                                              .port = ANY_PORT}),
               InvalidAddressException);
}

/**
 * Given a socket bound to a port
 * When a second socket is opened on the same address and port
 * Then it throws SocketOpenException
 */
TEST(UdpSocket, RefusesToOpenOnAPortAlreadyTaken) {
  NetworkContext context;
  const UdpSocket first{context, loopbackOnPort(ANY_PORT)};

  EXPECT_THROW(openSocketOn(context, first.localEndpoint()),
               SocketOpenException);
}
