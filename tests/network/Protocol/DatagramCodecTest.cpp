#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>
#include <vector>
#include "ByteSequence.hpp"
#include "DatagramCodec.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolFixtures.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network {
namespace {

using testing::byteSequence;
using testing::datagramOf;
using testing::defaultHeaderBytes;
using testing::EMPTY_MESSAGE_DATAGRAM_SIZE;
using testing::LARGEST_DATAGRAM_SIZE;
using testing::SOME_ACKNOWLEDGED_SEQUENCE;
using testing::SOME_ACKNOWLEDGEMENT_BITS;
using testing::SOME_FLAGS;
using testing::SOME_OTHER_PROTOCOL_ID;
using testing::SOME_OTHER_VERSION;
using testing::someSessionHeader;
using testing::VERSION_OFFSET;

template <typename Buffer>
concept SplittableAs = requires(Buffer&& buffer) {
  DatagramCodec::splitMessages(std::forward<Buffer>(buffer));
};

static_assert(SplittableAs<std::vector<std::byte>&>,
              "a datagram kept by the caller can be split");
static_assert(SplittableAs<const std::vector<std::byte>&>,
              "a constant datagram kept by the caller can be split");
static_assert(SplittableAs<std::span<const std::byte>>,
              "a view of a datagram kept by the caller can be split");
static_assert(!SplittableAs<std::vector<std::byte>>,
              "a temporary datagram cannot be split: its payloads would "
              "point into a destroyed buffer");
static_assert(!SplittableAs<const std::vector<std::byte>>,
              "a constant temporary datagram cannot be split either");
static_assert(!SplittableAs<std::array<std::byte, EMPTY_MESSAGE_DATAGRAM_SIZE>>,
              "a temporary array cannot be split either");

/**
 * Given a default header
 * When a datagram is encoded with it
 * Then it starts with RTYP, version 0, and zeros for every other field
 */
TEST(DatagramCodec, DefaultHeaderIsVersionZeroWithoutSession) {
  const auto datagram =
      DatagramCodec::encode(PacketHeader{}, MessageType::Heartbeat, {});

  EXPECT_TRUE(std::ranges::equal(std::span(datagram).first(PACKET_HEADER_SIZE),
                                 defaultHeaderBytes()));
}

/**
 * Given a header with a session token and a sequence, and a ReadyState payload
 * holding true
 * When the datagram is encoded
 * Then it is the 26 bytes of the protocol page: the header, the type 6, the
 * size 1 on two bytes, then the payload
 */
TEST(DatagramCodec, EncodesTheHeaderAndOneMessageByteByByte) {
  const auto datagram = DatagramCodec::encode(
      someSessionHeader(), MessageType::ReadyState, byteSequence({0x01}));

  EXPECT_EQ(datagram,
            byteSequence({0x52, 0x54, 0x59, 0x50, 0x00, 0x00, 0x01, 0x02, 0x03,
                          0x04, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x01, 0x01}));
}

/**
 * Given a message with an empty payload
 * When the datagram is encoded
 * Then it holds 25 bytes, the smallest datagram of the protocol
 */
TEST(DatagramCodec, EncodesAnEmptyMessageInTwentyFiveBytes) {
  const auto datagram =
      DatagramCodec::encode(PacketHeader{}, MessageType::Heartbeat, {});

  EXPECT_EQ(datagram.size(), EMPTY_MESSAGE_DATAGRAM_SIZE);
}

/**
 * Given a payload of MESSAGE_PAYLOAD_LIMIT bytes
 * When the datagram is encoded
 * Then it holds 1200 bytes, the largest datagram of the protocol
 */
TEST(DatagramCodec, EncodesTheLargestPayloadInTwelveHundredBytes) {
  const std::vector<std::byte> payload(MESSAGE_PAYLOAD_LIMIT);

  const auto datagram =
      DatagramCodec::encode(PacketHeader{}, MessageType::WorldState, payload);

  EXPECT_EQ(datagram.size(), LARGEST_DATAGRAM_SIZE);
}

/**
 * Given a payload one byte over MESSAGE_PAYLOAD_LIMIT
 * When the datagram is encoded
 * Then a DatagramTooLargeException is raised
 */
TEST(DatagramCodec, RefusesAPayloadOneByteTooLarge) {
  const std::vector<std::byte> payload(MESSAGE_PAYLOAD_LIMIT + 1);

  EXPECT_THROW((void)DatagramCodec::encode(PacketHeader{},
                                           MessageType::WorldState, payload),
               DatagramTooLargeException);
}

/**
 * Given a header whose seven fields all hold different values
 * When it is encoded then decoded
 * Then the decoded header equals the original, field by field
 */
TEST(DatagramCodec, DecodesTheHeaderItEncoded) {
  PacketHeader header = someSessionHeader();
  header.version = SOME_OTHER_VERSION;
  header.flags = SOME_FLAGS;
  header.acknowledgedSequence = SOME_ACKNOWLEDGED_SEQUENCE;
  header.acknowledgementBits = SOME_ACKNOWLEDGEMENT_BITS;
  const auto datagram =
      DatagramCodec::encode(header, MessageType::Heartbeat, {});

  EXPECT_EQ(DatagramCodec::decodeHeader(datagram), header);
}

/**
 * Given a datagram whose version byte is 1
 * When its header is decoded
 * Then the version 1 is returned, so the receiver can name it
 */
TEST(DatagramCodec, DecodesTheHeaderOfAnotherVersion) {
  auto datagram =
      DatagramCodec::encode(PacketHeader{}, MessageType::Heartbeat, {});
  datagram[VERSION_OFFSET] = std::byte{SOME_OTHER_VERSION};

  EXPECT_EQ(DatagramCodec::decodeHeader(datagram).version, SOME_OTHER_VERSION);
}

/**
 * Given the default header
 * When it is checked
 * Then it speaks this protocol
 */
TEST(DatagramCodec, SaysTheDefaultHeaderSpeaksThisProtocol) {
  EXPECT_TRUE(DatagramCodec::speaksThisProtocol(PacketHeader{}));
}

/**
 * Given a header of another version
 * When it is checked
 * Then it does not speak this protocol
 */
TEST(DatagramCodec, SaysAnotherVersionDoesNotSpeakThisProtocol) {
  PacketHeader header;
  header.version = SOME_OTHER_VERSION;

  EXPECT_FALSE(DatagramCodec::speaksThisProtocol(header));
}

/**
 * Given a header whose protocol id is not RTYP
 * When it is checked
 * Then it does not speak this protocol
 */
TEST(DatagramCodec, SaysAnotherProtocolIdDoesNotSpeakThisProtocol) {
  PacketHeader header;
  header.protocolId = SOME_OTHER_PROTOCOL_ID;

  EXPECT_FALSE(DatagramCodec::speaksThisProtocol(header));
}

/**
 * Given a header with a flag set, which version 0 never sets
 * When it is checked
 * Then it does not speak this protocol
 */
TEST(DatagramCodec, SaysAHeaderWithFlagsDoesNotSpeakThisProtocol) {
  PacketHeader header;
  header.flags = SOME_FLAGS;

  EXPECT_FALSE(DatagramCodec::speaksThisProtocol(header));
}

/**
 * Given exactly the 22 bytes of a header
 * When the header is decoded
 * Then it is the default header
 */
TEST(DatagramCodec, DecodesAHeaderOfExactlyTwentyTwoBytes) {
  EXPECT_EQ(DatagramCodec::decodeHeader(defaultHeaderBytes()), PacketHeader{});
}

/**
 * Given 21 bytes, one less than a header
 * When the header is decoded
 * Then a MalformedDatagramException is raised
 */
TEST(DatagramCodec, RefusesAHeaderOneByteShort) {
  auto bytes = defaultHeaderBytes();
  bytes.pop_back();

  EXPECT_THROW((void)DatagramCodec::decodeHeader(bytes),
               MalformedDatagramException);
}

/**
 * Given the ReadyState datagram of the protocol page
 * When its messages are split
 * Then there is one message of type 6 whose payload is the byte 01
 */
TEST(DatagramCodec, SplitsTheMessageOfADatagram) {
  const auto datagram = datagramOf({0x06, 0x00, 0x01, 0x01});

  const auto messages = DatagramCodec::splitMessages(datagram);

  ASSERT_EQ(messages.size(), 1U);
  EXPECT_EQ(messages.front().type,
            static_cast<std::uint8_t>(MessageType::ReadyState));
  EXPECT_TRUE(
      std::ranges::equal(messages.front().payload, byteSequence({0x01})));
}

/**
 * Given a datagram of 25 bytes holding a message without payload
 * When its messages are split
 * Then there is one message, with an empty payload
 */
TEST(DatagramCodec, SplitsAMessageWithoutPayload) {
  const auto datagram = datagramOf({0x05, 0x00, 0x00});

  const auto messages = DatagramCodec::splitMessages(datagram);

  ASSERT_EQ(messages.size(), 1U);
  EXPECT_TRUE(messages.front().payload.empty());
}

/**
 * Given a datagram holding a Heartbeat then a ReadyState
 * When its messages are split
 * Then both come back, in that order
 */
TEST(DatagramCodec, SplitsSeveralMessagesInOrder) {
  const auto datagram = datagramOf({0x05, 0x00, 0x00, 0x06, 0x00, 0x01, 0x01});

  const auto messages = DatagramCodec::splitMessages(datagram);

  ASSERT_EQ(messages.size(), 2U);
  EXPECT_EQ(messages[0].type,
            static_cast<std::uint8_t>(MessageType::Heartbeat));
  EXPECT_EQ(messages[1].type,
            static_cast<std::uint8_t>(MessageType::ReadyState));
}

/**
 * Given a datagram holding a message of a type no message has
 * When its messages are split
 * Then the message comes back with its type byte as it arrived
 */
TEST(DatagramCodec, KeepsTheTypeByteOfAnUnknownMessage) {
  const auto datagram = datagramOf({0xC8, 0x00, 0x00});

  const auto messages = DatagramCodec::splitMessages(datagram);

  ASSERT_EQ(messages.size(), 1U);
  EXPECT_EQ(messages.front().type, 0xC8);
}

/**
 * Given a datagram
 * When its messages are split
 * Then each payload is a view into the datagram, not a copy
 */
TEST(DatagramCodec, PayloadsPointIntoTheDatagram) {
  const auto datagram = datagramOf({0x06, 0x00, 0x01, 0x01});

  const auto messages = DatagramCodec::splitMessages(datagram);

  ASSERT_EQ(messages.size(), 1U);
  EXPECT_EQ(messages.front().payload.data(),
            std::span(datagram).subspan(EMPTY_MESSAGE_DATAGRAM_SIZE).data());
}

/**
 * Given a header alone, 22 bytes without any message
 * When its messages are split
 * Then a MalformedDatagramException is raised
 */
TEST(DatagramCodec, RefusesADatagramWithoutMessage) {
  const auto headerAlone = defaultHeaderBytes();

  EXPECT_THROW((void)DatagramCodec::splitMessages(headerAlone),
               MalformedDatagramException);
}

/**
 * Given 21 bytes, one less than a header
 * When its messages are split
 * Then a MalformedDatagramException is raised
 */
TEST(DatagramCodec, RefusesSplittingBytesShorterThanAHeader) {
  auto bytes = defaultHeaderBytes();
  bytes.pop_back();

  EXPECT_THROW((void)DatagramCodec::splitMessages(bytes),
               MalformedDatagramException);
}

/**
 * Given 24 bytes: a header and two of the three bytes of a message prefix
 * When its messages are split
 * Then a MalformedDatagramException is raised
 */
TEST(DatagramCodec, RefusesAMessagePrefixCutShort) {
  const auto datagram = datagramOf({0x05, 0x00});

  EXPECT_THROW((void)DatagramCodec::splitMessages(datagram),
               MalformedDatagramException);
}

/**
 * Given a message announcing two bytes of payload followed by only one
 * When the messages are split
 * Then a MalformedDatagramException is raised
 */
TEST(DatagramCodec, RefusesASizeRunningPastTheEnd) {
  const auto datagram = datagramOf({0x06, 0x00, 0x02, 0x01});

  EXPECT_THROW((void)DatagramCodec::splitMessages(datagram),
               MalformedDatagramException);
}

/**
 * Given a datagram of 25 bytes whose message announces 65535 bytes
 * When the messages are split
 * Then a MalformedDatagramException is raised
 */
TEST(DatagramCodec, RefusesTheLargestSizeInTheSmallestDatagram) {
  const auto datagram = datagramOf({0x06, 0xFF, 0xFF});

  EXPECT_THROW((void)DatagramCodec::splitMessages(datagram),
               MalformedDatagramException);
}

/**
 * Given a whole message followed by one more byte
 * When the messages are split
 * Then a MalformedDatagramException is raised
 */
TEST(DatagramCodec, RefusesBytesAfterTheLastMessage) {
  const auto datagram = datagramOf({0x06, 0x00, 0x01, 0x01, 0x00});

  EXPECT_THROW((void)DatagramCodec::splitMessages(datagram),
               MalformedDatagramException);
}

/**
 * Given a whole message followed by a second one cut short
 * When the messages are split
 * Then a MalformedDatagramException is raised, so the first is not delivered
 * either
 */
TEST(DatagramCodec, RefusesAWholeDatagramWhenItsSecondMessageIsCutShort) {
  const auto datagram = datagramOf({0x06, 0x00, 0x01, 0x01, 0x06, 0x00, 0x01});

  EXPECT_THROW((void)DatagramCodec::splitMessages(datagram),
               MalformedDatagramException);
}

/**
 * Given a datagram encoded with a payload
 * When its messages are split
 * Then the single message has the type and the payload it was encoded with
 */
TEST(DatagramCodec, SplitsWhatItEncoded) {
  const auto payload = byteSequence({0x00, 0x00, 0x00, 0x01, 0x11});
  const auto datagram =
      DatagramCodec::encode(PacketHeader{}, MessageType::PlayerInput, payload);

  const auto messages = DatagramCodec::splitMessages(datagram);

  ASSERT_EQ(messages.size(), 1U);
  EXPECT_EQ(messages.front().type,
            static_cast<std::uint8_t>(MessageType::PlayerInput));
  EXPECT_TRUE(std::ranges::equal(messages.front().payload, payload));
}

}  // namespace
}  // namespace rtype::network
