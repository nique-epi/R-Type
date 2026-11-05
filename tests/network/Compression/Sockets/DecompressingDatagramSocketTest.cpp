#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "CompressingDatagramSocket.hpp"
#include "DatagramCompressionFixtures.hpp"
#include "DecompressingDatagramSocket.hpp"
#include "ReceivedDatagrams.hpp"
#include "RecordingDatagramSocket.hpp"

using rtype::network::CompressingDatagramSocket;
using rtype::network::DecompressingDatagramSocket;
using rtype::network::testing::codedDatagramWithoutSession;
using rtype::network::testing::codedSessionDatagram;
using rtype::network::testing::handshakeHeader;
using rtype::network::testing::peer;
using rtype::network::testing::ReceivedDatagrams;
using rtype::network::testing::sessionHeader;
using rtype::network::testing::worldStateDatagram;

/**
 * Given a decompressing socket that is receiving
 * When the coded form of a session datagram arrives
 * Then the handler gets the original datagram, flag cleared
 */
TEST(DecompressingDatagramSocket, HandsOnTheOriginalOfACodedDatagram) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& real = *recording;
  DecompressingDatagramSocket socket{std::move(recording)};
  ReceivedDatagrams received;
  socket.startReceiving(received.handler());

  real.receive(peer(), codedSessionDatagram());

  ASSERT_EQ(received.all().size(), 1U);
  EXPECT_EQ(received.all().front(), worldStateDatagram(sessionHeader()));
}

/**
 * Given a decompressing socket that is receiving
 * When a plain datagram arrives
 * Then the handler gets it as it is
 */
TEST(DecompressingDatagramSocket, HandsOnAPlainDatagramAsItIs) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& real = *recording;
  DecompressingDatagramSocket socket{std::move(recording)};
  ReceivedDatagrams received;
  socket.startReceiving(received.handler());
  const std::vector<std::byte> plain = worldStateDatagram(handshakeHeader());

  real.receive(peer(), plain);

  ASSERT_EQ(received.all().size(), 1U);
  EXPECT_EQ(received.all().front(), plain);
}

/**
 * Given a decompressing socket that is receiving
 * When a coded datagram arrives with its last byte cut off
 * Then the handler gets nothing
 */
TEST(DecompressingDatagramSocket, DropsACodedDatagramThatDoesNotDecode) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& real = *recording;
  DecompressingDatagramSocket socket{std::move(recording)};
  ReceivedDatagrams received;
  socket.startReceiving(received.handler());
  std::vector<std::byte> coded = codedSessionDatagram();
  coded.pop_back();

  real.receive(peer(), coded);

  EXPECT_TRUE(received.all().empty());
}

/**
 * Given a decompressing socket that is receiving
 * When a coded datagram arrives with the token 0
 * Then the handler gets nothing, since nobody codes before a session
 */
TEST(DecompressingDatagramSocket, DropsACodedDatagramWithoutASession) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& real = *recording;
  DecompressingDatagramSocket socket{std::move(recording)};
  ReceivedDatagrams received;
  socket.startReceiving(received.handler());

  real.receive(peer(), codedDatagramWithoutSession());

  EXPECT_TRUE(received.all().empty());
}

/**
 * Given a decompressing socket over a recording socket
 * When it sends a session datagram
 * Then the wrapped socket sends it as it is
 */
TEST(DecompressingDatagramSocket, SendsEverythingAsItIs) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  const RecordingDatagramSocket& real = *recording;
  DecompressingDatagramSocket socket{std::move(recording)};
  const std::vector<std::byte> plain = worldStateDatagram(sessionHeader());

  socket.send(peer(), plain);

  ASSERT_EQ(real.sent().size(), 1U);
  EXPECT_EQ(real.sent().front().payload, plain);
}

/**
 * Given the socket of a server and the socket of a client
 * When the server sends a session datagram and the client receives what went
 *      out
 * Then the client's handler gets the datagram the server sent
 */
TEST(DecompressingDatagramSocket, ReadsWhatTheCompressingSocketSent) {
  auto serverWire = std::make_unique<RecordingDatagramSocket>();
  const RecordingDatagramSocket& wire = *serverWire;
  CompressingDatagramSocket server{std::move(serverWire)};
  auto clientWire = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& arrival = *clientWire;
  DecompressingDatagramSocket client{std::move(clientWire)};
  ReceivedDatagrams received;
  client.startReceiving(received.handler());
  const std::vector<std::byte> plain = worldStateDatagram(sessionHeader());

  server.send(peer(), plain);
  arrival.receive(peer(), wire.sent().front().payload);

  ASSERT_EQ(received.all().size(), 1U);
  EXPECT_EQ(received.all().front(), plain);
}
