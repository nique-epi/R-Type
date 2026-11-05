#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <span>
#include <vector>
#include "DatagramCompression.hpp"
#include "DatagramCompressionFixtures.hpp"
#include "DatagramCompressionTestConstants.hpp"
#include "ProtocolConstants.hpp"

using rtype::network::COMPRESSED_FLAG;
using rtype::network::compressedForm;
using rtype::network::FLAGS_OFFSET;
using rtype::network::isCompressed;
using rtype::network::PACKET_HEADER_SIZE;
using rtype::network::plainForm;
using rtype::network::testing::ANOTHER_VERSION;
using rtype::network::testing::codedDatagramWithoutSession;
using rtype::network::testing::codedSessionDatagram;
using rtype::network::testing::handshakeHeader;
using rtype::network::testing::incompressibleDatagram;
using rtype::network::testing::sessionHeader;
using rtype::network::testing::VERSION_OFFSET;
using rtype::network::testing::worldStateDatagram;

/**
 * Given a WorldState datagram of a session
 * When its compressed form is asked for
 * Then it is shorter than the original and has COMPRESSED_FLAG raised
 */
TEST(DatagramCompression, CodesTheBodyOfASessionDatagram) {
  const std::vector<std::byte> plain = worldStateDatagram(sessionHeader());

  const auto coded = compressedForm(plain);

  ASSERT_TRUE(coded.has_value());
  const std::vector<std::byte> bytes = coded.value_or(std::vector<std::byte>{});
  EXPECT_LT(bytes.size(), plain.size());
  EXPECT_NE(bytes[FLAGS_OFFSET] & std::byte{COMPRESSED_FLAG}, std::byte{0});
}

/**
 * Given a WorldState datagram of a session
 * When its compressed form is asked for
 * Then the header is the original one, but for the flag that was raised
 */
TEST(DatagramCompression, ChangesNothingInTheHeaderButTheFlag) {
  const std::vector<std::byte> plain = worldStateDatagram(sessionHeader());
  std::vector<std::byte> expected(plain.begin(),
                                  plain.begin() + PACKET_HEADER_SIZE);
  expected[FLAGS_OFFSET] |= std::byte{COMPRESSED_FLAG};

  const auto coded = compressedForm(plain);

  ASSERT_TRUE(coded.has_value());
  const std::vector<std::byte> bytes = coded.value_or(std::vector<std::byte>{});
  EXPECT_EQ(
      std::vector<std::byte>(bytes.begin(), bytes.begin() + PACKET_HEADER_SIZE),
      expected);
}

/**
 * Given a datagram sent before a session, which carries the token 0
 * When its compressed form is asked for
 * Then there is none, so a peer of any version can read it
 */
TEST(DatagramCompression, LeavesAHandshakeDatagramAlone) {
  EXPECT_FALSE(
      compressedForm(worldStateDatagram(handshakeHeader())).has_value());
}

/**
 * Given a datagram that is all header
 * When its compressed form is asked for
 * Then there is none
 */
TEST(DatagramCompression, LeavesADatagramWithoutABodyAlone) {
  const std::vector<std::byte> plain = worldStateDatagram(sessionHeader());

  EXPECT_FALSE(
      compressedForm(std::span(plain).first(PACKET_HEADER_SIZE)).has_value());
}

/**
 * Given a datagram that is already coded
 * When its compressed form is asked for
 * Then there is none, so a body is never coded twice
 */
TEST(DatagramCompression, LeavesACodedDatagramAlone) {
  EXPECT_FALSE(compressedForm(codedSessionDatagram()).has_value());
}

/**
 * Given a session datagram whose body is random bytes
 * When its compressed form is asked for
 * Then there is none, since the code would not make it shorter
 */
TEST(DatagramCompression, LeavesADatagramThatWouldNotShrinkAlone) {
  EXPECT_FALSE(
      compressedForm(incompressibleDatagram(sessionHeader())).has_value());
}

/**
 * Given the coded form of a session datagram
 * When its plain form is asked for
 * Then it is the original datagram, byte for byte
 */
TEST(DatagramCompression, GivesBackTheOriginalDatagram) {
  const std::vector<std::byte> plain = worldStateDatagram(sessionHeader());

  const auto restored = plainForm(codedSessionDatagram());

  ASSERT_TRUE(restored.has_value());
  EXPECT_EQ(restored.value_or(std::vector<std::byte>{}), plain);
}

/**
 * Given a coded datagram whose token was set to 0
 * When its plain form is asked for
 * Then there is none, since nobody codes a datagram before a session
 */
TEST(DatagramCompression, RefusesACodedDatagramWithoutASession) {
  EXPECT_FALSE(plainForm(codedDatagramWithoutSession()).has_value());
}

/**
 * Given a coded datagram with its last byte cut off
 * When its plain form is asked for
 * Then there is none
 */
TEST(DatagramCompression, RefusesACodedBodyThatIsCut) {
  std::vector<std::byte> coded = codedSessionDatagram();
  coded.pop_back();

  EXPECT_FALSE(plainForm(coded).has_value());
}

/**
 * Given the coded form of a session datagram
 * When it is asked whether it is compressed
 * Then it is
 */
TEST(DatagramCompression, SaysACodedDatagramIsCompressed) {
  EXPECT_TRUE(isCompressed(codedSessionDatagram()));
}

/**
 * Given a plain session datagram
 * When it is asked whether it is compressed
 * Then it is not
 */
TEST(DatagramCompression, SaysAPlainDatagramIsNotCompressed) {
  EXPECT_FALSE(isCompressed(worldStateDatagram(sessionHeader())));
}

/**
 * Given a coded datagram whose version byte names another version
 * When it is asked whether it is compressed
 * Then it is not, so a peer of another version is left to the usual checks
 */
TEST(DatagramCompression, SaysADatagramOfAnotherVersionIsNotCompressed) {
  std::vector<std::byte> coded = codedSessionDatagram();
  coded[VERSION_OFFSET] = std::byte{ANOTHER_VERSION};

  EXPECT_FALSE(isCompressed(coded));
}

/**
 * Given a datagram that is all header, and one shorter than a header
 * When each is asked whether it is compressed
 * Then neither is, and nothing is thrown
 */
TEST(DatagramCompression, SaysADatagramWithoutABodyIsNotCompressed) {
  const std::vector<std::byte> coded = codedSessionDatagram();

  EXPECT_FALSE(isCompressed(std::span(coded).first(PACKET_HEADER_SIZE)));
  EXPECT_FALSE(isCompressed(std::span(coded).first(PACKET_HEADER_SIZE - 1)));
}
