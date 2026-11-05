#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "CompressingDatagramSocket.hpp"
#include "DatagramCompression.hpp"
#include "DatagramCompressionFixtures.hpp"
#include "ReceivedDatagrams.hpp"
#include "RecordingDatagramSocket.hpp"

using rtype::network::CompressingDatagramSocket;
using rtype::network::isCompressed;
using rtype::network::testing::codedSessionDatagram;
using rtype::network::testing::handshakeHeader;
using rtype::network::testing::peer;
using rtype::network::testing::ReceivedDatagrams;
using rtype::network::testing::sessionHeader;
using rtype::network::testing::worldStateDatagram;

/**
 * Given a compressing socket over a recording socket
 * When it sends a WorldState datagram of a session
 * Then the wrapped socket sends it coded and shorter, to the same peer
 */
TEST(CompressingDatagramSocket, SendsASessionDatagramCoded) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  const RecordingDatagramSocket& real = *recording;
  CompressingDatagramSocket socket{std::move(recording)};
  const std::vector<std::byte> plain = worldStateDatagram(sessionHeader());

  socket.send(peer(), plain);

  ASSERT_EQ(real.sent().size(), 1U);
  EXPECT_EQ(real.sent().front().destination, peer());
  EXPECT_TRUE(isCompressed(real.sent().front().payload));
  EXPECT_LT(real.sent().front().payload.size(), plain.size());
}

/**
 * Given a compressing socket over a recording socket
 * When it sends a datagram of the handshake, which carries the token 0
 * Then the wrapped socket sends it as it is
 */
TEST(CompressingDatagramSocket, SendsAHandshakeDatagramAsItIs) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  const RecordingDatagramSocket& real = *recording;
  CompressingDatagramSocket socket{std::move(recording)};
  const std::vector<std::byte> plain = worldStateDatagram(handshakeHeader());

  socket.send(peer(), plain);

  ASSERT_EQ(real.sent().size(), 1U);
  EXPECT_EQ(real.sent().front().payload, plain);
}

/**
 * Given a compressing socket that is receiving
 * When a datagram arrives with COMPRESSED_FLAG raised
 * Then the handler gets nothing, since a client never codes what it sends
 */
TEST(CompressingDatagramSocket, DropsADatagramReceivedWithTheFlag) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& real = *recording;
  CompressingDatagramSocket socket{std::move(recording)};
  ReceivedDatagrams received;
  socket.startReceiving(received.handler());

  real.receive(peer(), codedSessionDatagram());

  EXPECT_TRUE(received.all().empty());
}

/**
 * Given a compressing socket that is receiving
 * When a plain datagram arrives
 * Then the handler gets it as it is
 */
TEST(CompressingDatagramSocket, HandsOnAPlainDatagramReceived) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  RecordingDatagramSocket& real = *recording;
  CompressingDatagramSocket socket{std::move(recording)};
  ReceivedDatagrams received;
  socket.startReceiving(received.handler());
  const std::vector<std::byte> plain = worldStateDatagram(sessionHeader());

  real.receive(peer(), plain);

  ASSERT_EQ(received.all().size(), 1U);
  EXPECT_EQ(received.all().front(), plain);
}
