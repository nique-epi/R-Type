#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ByteSequence.hpp"
#include "DatagramCodec.hpp"
#include "DatagramCodecFixture.hpp"
#include "DatagramCodecFixtures.hpp"
#include "EncodedMessage.hpp"
#include "MessageFrame.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolFixtures.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network {

using testing::BOTH_FLAGS_OF_VERSION_TWO;
using testing::byteSequence;
using testing::DatagramCodecFixture;
using testing::datagramOf;
using testing::defaultHeaderBytes;
using testing::EMPTY_MESSAGE_DATAGRAM_SIZE;
using testing::FILLING_PAYLOAD_SIZE;
using testing::LARGEST_DATAGRAM_SIZE;
using testing::malformedDatagrams;
using testing::nameOfFault;
using testing::OVERFILLING_PAYLOAD_SIZE;
using testing::SOME_ACKNOWLEDGED_SEQUENCE;
using testing::SOME_ACKNOWLEDGEMENT_BITS;
using testing::SOME_FLAGS;
using testing::SOME_OTHER_PROTOCOL_ID;
using testing::SOME_OTHER_VERSION;
using testing::someSessionHeader;
using testing::VERSION_OFFSET;

/**
 * Given a default header
 * When a datagram is encoded with it
 * Then it starts with RTYP, version 2, and zeros for every other field
 */
TEST(DatagramCodec, DefaultHeaderIsVersionTwoWithoutSession) {
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
            byteSequence({0x52, 0x54, 0x59, 0x50, 0x02, 0x00, 0x01, 0x02, 0x03,
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
 * Given a datagram whose version byte is 3
 * When its header is decoded
 * Then the version 3 is returned, so the receiver can name it
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
 * Given a header with the two flags version 2 knows
 * When it is checked
 * Then it speaks this protocol
 */
TEST(DatagramCodec, SaysAHeaderWithTheFlagsOfThisVersionSpeaksThisProtocol) {
  PacketHeader header;
  header.flags = BOTH_FLAGS_OF_VERSION_TWO;

  EXPECT_TRUE(DatagramCodec::speaksThisProtocol(header));
}

/**
 * Given a header with a flag version 2 does not know
 * When it is checked
 * Then it does not speak this protocol
 */
TEST(DatagramCodec, SaysAHeaderWithAnUnknownFlagDoesNotSpeakThisProtocol) {
  PacketHeader header;
  header.flags = SOME_FLAGS;

  EXPECT_FALSE(DatagramCodec::speaksThisProtocol(header));
}

/**
 * Given a header of this protocol id with another version, and flags set
 * When it is checked
 * Then it speaks another version: its flags may mean something there
 */
TEST(DatagramCodec, SaysAHeaderOfAnotherVersionSpeaksAnotherVersion) {
  PacketHeader header;
  header.version = SOME_OTHER_VERSION;
  header.flags = SOME_FLAGS;

  EXPECT_TRUE(DatagramCodec::speaksAnotherVersion(header));
}

/**
 * Given the default header
 * When it is checked
 * Then it does not speak another version
 */
TEST(DatagramCodec, SaysTheDefaultHeaderSpeaksNoOtherVersion) {
  EXPECT_FALSE(DatagramCodec::speaksAnotherVersion(PacketHeader{}));
}

/**
 * Given a header of another protocol id and another version
 * When it is checked
 * Then it does not speak another version of this protocol
 */
TEST(DatagramCodec, SaysAnotherProtocolSpeaksNoOtherVersion) {
  PacketHeader header;
  header.protocolId = SOME_OTHER_PROTOCOL_ID;
  header.version = SOME_OTHER_VERSION;

  EXPECT_FALSE(DatagramCodec::speaksAnotherVersion(header));
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

/**
 * Given a datagram holding a Heartbeat then a ReadyState
 * When its message types are read
 * Then those two types are set, and no other
 */
TEST(DatagramCodec, ReadsTheTypesOfEveryMessage) {
  const auto datagram = datagramOf({0x05, 0x00, 0x00, 0x06, 0x00, 0x01, 0x01});
  DatagramCodec::MessageTypes expected;
  expected.set(static_cast<std::size_t>(MessageType::Heartbeat));
  expected.set(static_cast<std::size_t>(MessageType::ReadyState));

  EXPECT_EQ(DatagramCodec::messageTypes(datagram), expected);
}

/**
 * Given a Heartbeat and an EntityDestroyed payload
 * When they are encoded in one datagram
 * Then it splits back into both, in order, with their payloads
 */
TEST(DatagramCodec, EncodesSeveralMessagesInOrder) {
  const std::vector<EncodedMessage> messages{
      EncodedMessage{.type = MessageType::Heartbeat, .payload = {}},
      EncodedMessage{.type = MessageType::EntityDestroyed,
                     .payload = std::vector<std::byte>(FILLING_PAYLOAD_SIZE)}};

  const std::vector<std::byte> datagram =
      DatagramCodec::encode(PacketHeader{}, messages);

  const std::vector<MessageFrame> frames =
      DatagramCodec::splitMessages(datagram);
  ASSERT_EQ(frames.size(), 2U);
  EXPECT_EQ(frames.front().type,
            static_cast<std::uint8_t>(MessageType::Heartbeat));
  EXPECT_TRUE(frames.front().payload.empty());
  EXPECT_EQ(frames.back().type,
            static_cast<std::uint8_t>(MessageType::EntityDestroyed));
  EXPECT_EQ(frames.back().payload.size(), FILLING_PAYLOAD_SIZE);
}

/**
 * Given two messages that make exactly LARGEST_DATAGRAM_SIZE bytes with the
 * header
 * When they are encoded
 * Then the datagram is LARGEST_DATAGRAM_SIZE bytes
 */
TEST(DatagramCodec, EncodesSeveralMessagesThatFillADatagram) {
  const std::vector<EncodedMessage> messages(
      2,
      EncodedMessage{.type = MessageType::GameEvent,
                     .payload = std::vector<std::byte>(FILLING_PAYLOAD_SIZE)});

  EXPECT_EQ(DatagramCodec::encode(PacketHeader{}, messages).size(),
            LARGEST_DATAGRAM_SIZE);
}

/**
 * Given two messages that make one byte more than LARGEST_DATAGRAM_SIZE
 * When they are encoded
 * Then DatagramTooLargeException is thrown
 */
TEST(DatagramCodec, RefusesSeveralMessagesOneByteTooLarge) {
  const std::vector<EncodedMessage> messages{
      EncodedMessage{.type = MessageType::GameEvent,
                     .payload = std::vector<std::byte>(FILLING_PAYLOAD_SIZE)},
      EncodedMessage{
          .type = MessageType::GameEvent,
          .payload = std::vector<std::byte>(OVERFILLING_PAYLOAD_SIZE)}};

  EXPECT_THROW(
      static_cast<void>(DatagramCodec::encode(PacketHeader{}, messages)),
      DatagramTooLargeException);
}

/**
 * Given no message
 * When a datagram of them is encoded
 * Then EmptyDatagramException is thrown
 */
TEST(DatagramCodec, RefusesADatagramWithoutAMessage) {
  const std::vector<EncodedMessage> none;

  EXPECT_THROW(static_cast<void>(DatagramCodec::encode(PacketHeader{}, none)),
               EmptyDatagramException);
}

/**
 * Given bytes splitMessages() refuses
 * When their message types are read
 * Then there are none, and nothing is thrown
 */
TEST_P(DatagramCodecFixture, HasNoMessageTypes) {
  ASSERT_THROW((void)DatagramCodec::splitMessages(GetParam().bytes),
               MalformedDatagramException);

  EXPECT_EQ(DatagramCodec::messageTypes(GetParam().bytes), std::nullopt);
}

INSTANTIATE_TEST_SUITE_P(EveryFaultOfSplitMessages, DatagramCodecFixture,
                         ::testing::ValuesIn(malformedDatagrams()),
                         nameOfFault);

}  // namespace rtype::network
