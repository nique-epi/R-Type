#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Heartbeat.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network {
namespace {

using testing::byteSequence;
using testing::CHALLENGE_DATAGRAM_SIZE_ON_THE_PAGE;
using testing::FOUR_ROCKETS;
using testing::LAST_PLAYER_NUMBER;
using testing::ONE_BYTE_NICKNAME;
using testing::PLAYER_NUMBER_ABOVE_THE_LAST;
using testing::REFUSAL_REASON_ABOVE_THE_LAST;
using testing::SEVENTEEN_BYTE_NICKNAME;
using testing::SMALLEST_REQUEST_DATAGRAM_SIZE_ON_THE_PAGE;
using testing::SOME_COOKIE;

ConnectionRequest smallestRequest() {
  return ConnectionRequest{.cookie = 0,
                           .nickname = std::string{ONE_BYTE_NICKNAME}};
}

/**
 * Given a request with a cookie and the nickname "A"
 * When it is encoded
 * Then the payload is the cookie on eight bytes, then the length 1 and the
 * letter A
 */
TEST(ConnectionRequest, EncodesTheCookieThenTheNicknameWithItsLength) {
  const auto expected = byteSequence(
      {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x41});

  EXPECT_EQ(
      encodeMessage(ConnectionRequest{
          .cookie = SOME_COOKIE, .nickname = std::string{ONE_BYTE_NICKNAME}}),
      expected);
}

/**
 * Given a request whose nickname is sixteen bytes of four-byte characters
 * When it is encoded then decoded
 * Then the decoded request equals the original
 */
TEST(ConnectionRequest, DecodesWhatItEncoded) {
  const ConnectionRequest request{.cookie = SOME_COOKIE,
                                  .nickname = std::string{FOUR_ROCKETS}};

  EXPECT_EQ(decodeMessage<ConnectionRequest>(encodeMessage(request)), request);
}

/**
 * Given the bytes of a request whose nickname is an accented letter
 * When they are decoded then encoded again
 * Then the same bytes come back
 */
TEST(ConnectionRequest, EncodesAgainTheBytesItDecoded) {
  const auto bytes = byteSequence(
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xC3, 0xA9});

  EXPECT_EQ(encodeMessage(decodeMessage<ConnectionRequest>(bytes)), bytes);
}

/**
 * Given a request with an empty nickname
 * When it is encoded
 * Then an InvalidNicknameException is raised
 */
TEST(ConnectionRequest, RefusesToWriteAnEmptyNickname) {
  ByteWriter writer;

  EXPECT_THROW(
      encodePayload(writer, ConnectionRequest{.cookie = 0, .nickname = ""}),
      InvalidNicknameException);
}

/**
 * Given a request whose nickname is 17 bytes long
 * When writing it is refused
 * Then nothing was written, not even the cookie that comes first
 */
TEST(ConnectionRequest, RefusedRequestWritesNothing) {
  ByteWriter writer;

  EXPECT_THROW(
      encodePayload(
          writer,
          ConnectionRequest{.cookie = SOME_COOKIE,
                            .nickname = std::string{SEVENTEEN_BYTE_NICKNAME}}),
      InvalidNicknameException);

  EXPECT_TRUE(writer.bytes().empty());
}

/**
 * Given a payload whose nickname is empty
 * When it is decoded
 * Then an InvalidNicknameException is raised
 */
TEST(ConnectionRequest, RefusesToReadAnEmptyNickname) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               InvalidNicknameException);
}

/**
 * Given a payload whose nickname is 17 bytes long
 * When it is decoded
 * Then an InvalidNicknameException is raised
 */
TEST(ConnectionRequest, RefusesToReadASeventeenByteNickname) {
  ByteWriter writer;
  writer.writeUint64(SOME_COOKIE);
  writer.writeString(SEVENTEEN_BYTE_NICKNAME);

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(writer.bytes()),
               InvalidNicknameException);
}

/**
 * Given a payload whose nickname is the overlong form C0 80
 * When it is decoded
 * Then an InvalidNicknameException is raised
 */
TEST(ConnectionRequest, RefusesToReadANicknameThatIsNotUtf8) {
  const auto bytes = byteSequence(
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xC0, 0x80});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               InvalidNicknameException);
}

/**
 * Given a payload of seven bytes, one less than the cookie
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(ConnectionRequest, RefusesARequestCutShort) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               BufferUnderflowException);
}

/**
 * Given a whole request followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(ConnectionRequest, RefusesBytesAfterTheNickname) {
  const auto bytes = byteSequence(
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x41, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               TrailingBytesException);
}

/**
 * Given a request with a one-byte nickname, the smallest there is
 * When its datagram is encoded
 * Then it holds the 35 bytes the protocol page gives
 */
TEST(ConnectionRequest, SmallestRequestDatagramHoldsThirtyFiveBytes) {
  EXPECT_EQ(DatagramCodec::encode(PacketHeader{}, smallestRequest()).size(),
            SMALLEST_REQUEST_DATAGRAM_SIZE_ON_THE_PAGE);
}

/**
 * Given a challenge
 * When it is encoded
 * Then the payload is the cookie on eight bytes
 */
TEST(ConnectionChallenge, EncodesTheCookie) {
  const auto expected =
      byteSequence({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});

  EXPECT_EQ(encodeMessage(ConnectionChallenge{.cookie = SOME_COOKIE}),
            expected);
}

/**
 * Given a challenge
 * When it is encoded then decoded
 * Then the decoded challenge equals the original
 */
TEST(ConnectionChallenge, DecodesWhatItEncoded) {
  const ConnectionChallenge challenge{.cookie = SOME_COOKIE};

  EXPECT_EQ(decodeMessage<ConnectionChallenge>(encodeMessage(challenge)),
            challenge);
}

/**
 * Given a payload of seven bytes
 * When it is decoded as a challenge
 * Then a BufferUnderflowException is raised
 */
TEST(ConnectionChallenge, RefusesAChallengeCutShort) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionChallenge>(bytes),
               BufferUnderflowException);
}

/**
 * Given a cookie followed by one more byte
 * When it is decoded as a challenge
 * Then a TrailingBytesException is raised
 */
TEST(ConnectionChallenge, RefusesBytesAfterTheCookie) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionChallenge>(bytes),
               TrailingBytesException);
}

/**
 * Given a challenge
 * When its datagram is encoded
 * Then it holds the 33 bytes the protocol page gives
 */
TEST(ConnectionChallenge, ChallengeDatagramHoldsThirtyThreeBytes) {
  EXPECT_EQ(DatagramCodec::encode(PacketHeader{},
                                  ConnectionChallenge{.cookie = SOME_COOKIE})
                .size(),
            CHALLENGE_DATAGRAM_SIZE_ON_THE_PAGE);
}

/**
 * Given the smallest request a sender can send, and every answer the server
 * gives a sender it does not know yet
 * When their datagrams are encoded
 * Then no answer is larger than the request, so the server cannot be used to
 * send a victim more bytes than the forger sent
 */
TEST(ConnectionChallenge, NoAnswerIsLargerThanTheSmallestRequest) {
  const std::size_t requestSize =
      DatagramCodec::encode(PacketHeader{}, smallestRequest()).size();

  EXPECT_LE(DatagramCodec::encode(PacketHeader{},
                                  ConnectionChallenge{.cookie = SOME_COOKIE})
                .size(),
            requestSize);
  EXPECT_LE(
      DatagramCodec::encode(
          PacketHeader{}, ConnectionRefused{.reason = RefusalReason::GameFull})
          .size(),
      requestSize);
  EXPECT_LE(DatagramCodec::encode(
                PacketHeader{},
                ConnectionAccepted{.playerNumber = LAST_PLAYER_NUMBER})
                .size(),
            requestSize);
}

/**
 * Given the acceptance of player 1
 * When it is encoded
 * Then the payload is the byte 01
 */
TEST(ConnectionAccepted, EncodesThePlayerNumber) {
  const auto expected = byteSequence({0x01});

  EXPECT_EQ(encodeMessage(ConnectionAccepted{.playerNumber = 1}), expected);
}

/**
 * Given the payloads 01 and 04, the first and the last player of a game
 * When each is decoded
 * Then each gives its player number
 */
TEST(ConnectionAccepted, AcceptsTheFirstAndTheLastPlayerNumbers) {
  const auto first = byteSequence({0x01});
  const auto last = byteSequence({0x04});

  EXPECT_EQ(decodeMessage<ConnectionAccepted>(first).playerNumber, 1);
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(last).playerNumber,
            LAST_PLAYER_NUMBER);
}

/**
 * Given the payloads 00 and 05, just outside the players of a game
 * When each is decoded
 * Then each is refused
 */
TEST(ConnectionAccepted, RefusesToReadAPlayerNumberOutsideOneToFour) {
  const auto belowTheFirst = byteSequence({0x00});
  const auto aboveTheLast = byteSequence({0x05});

  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(belowTheFirst),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(aboveTheLast),
               InvalidFieldValueException);
}

/**
 * Given the acceptance of a fifth player
 * When it is encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(ConnectionAccepted, RefusesToWriteAPlayerNumberOutsideOneToFour) {
  EXPECT_THROW((void)encodeMessage(ConnectionAccepted{
                   .playerNumber = PLAYER_NUMBER_ABOVE_THE_LAST}),
               InvalidFieldValueException);
}

/**
 * Given an empty payload
 * When it is decoded as an acceptance
 * Then a BufferUnderflowException is raised
 */
TEST(ConnectionAccepted, RefusesAnEmptyPayload) {
  const std::vector<std::byte> empty;

  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(empty),
               BufferUnderflowException);
}

/**
 * Given a player number followed by one more byte
 * When it is decoded as an acceptance
 * Then a TrailingBytesException is raised
 */
TEST(ConnectionAccepted, RefusesBytesAfterThePlayerNumber) {
  const auto bytes = byteSequence({0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(bytes),
               TrailingBytesException);
}

/**
 * Given a refusal because a game is in progress
 * When it is encoded
 * Then the payload is the byte 02
 */
TEST(ConnectionRefused, EncodesTheReason) {
  const auto expected = byteSequence({0x02});

  EXPECT_EQ(
      encodeMessage(ConnectionRefused{.reason = RefusalReason::GameInProgress}),
      expected);
}

/**
 * Given the payloads 01, 02 and 03
 * When each is decoded
 * Then they give GameFull, GameInProgress and IncompatibleVersion
 */
TEST(ConnectionRefused, DecodesEveryReasonOfTheProtocolPage) {
  const auto gameFull = byteSequence({0x01});
  const auto gameInProgress = byteSequence({0x02});
  const auto incompatibleVersion = byteSequence({0x03});

  EXPECT_EQ(decodeMessage<ConnectionRefused>(gameFull).reason,
            RefusalReason::GameFull);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(gameInProgress).reason,
            RefusalReason::GameInProgress);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(incompatibleVersion).reason,
            RefusalReason::IncompatibleVersion);
}

/**
 * Given the payloads 00 and 04, just outside the reasons of the protocol page
 * When each is decoded
 * Then each is refused
 */
TEST(ConnectionRefused, RefusesToReadAReasonOutsideTheList) {
  const auto belowTheFirst = byteSequence({0x00});
  const auto aboveTheLast = byteSequence({0x04});

  EXPECT_THROW((void)decodeMessage<ConnectionRefused>(belowTheFirst),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<ConnectionRefused>(aboveTheLast),
               InvalidFieldValueException);
}

/**
 * Given a refusal whose reason is not on the protocol page
 * When it is encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(ConnectionRefused, RefusesToWriteAReasonOutsideTheList) {
  // NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange)
  const auto unlisted =
      static_cast<RefusalReason>(REFUSAL_REASON_ABOVE_THE_LAST);
  // NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange)

  EXPECT_THROW((void)encodeMessage(ConnectionRefused{.reason = unlisted}),
               InvalidFieldValueException);
}

/**
 * Given a reason followed by one more byte
 * When it is decoded as a refusal
 * Then a TrailingBytesException is raised
 */
TEST(ConnectionRefused, RefusesBytesAfterTheReason) {
  const auto bytes = byteSequence({0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionRefused>(bytes),
               TrailingBytesException);
}

/**
 * Given a disconnection
 * When it is encoded
 * Then the payload is empty
 */
TEST(Disconnection, EncodesAnEmptyPayload) {
  EXPECT_TRUE(encodeMessage(Disconnection{}).empty());
}

/**
 * Given an empty payload
 * When it is decoded as a disconnection
 * Then a disconnection comes back
 */
TEST(Disconnection, DecodesAnEmptyPayload) {
  const std::vector<std::byte> empty;

  EXPECT_EQ(decodeMessage<Disconnection>(empty), Disconnection{});
}

/**
 * Given a payload of one byte
 * When it is decoded as a disconnection
 * Then a TrailingBytesException is raised
 */
TEST(Disconnection, RefusesAnyByteInThePayload) {
  const auto bytes = byteSequence({0x00});

  EXPECT_THROW((void)decodeMessage<Disconnection>(bytes),
               TrailingBytesException);
}

/**
 * Given a heartbeat
 * When it is encoded
 * Then the payload is empty
 */
TEST(Heartbeat, EncodesAnEmptyPayload) {
  EXPECT_TRUE(encodeMessage(Heartbeat{}).empty());
}

/**
 * Given an empty payload
 * When it is decoded as a heartbeat
 * Then a heartbeat comes back
 */
TEST(Heartbeat, DecodesAnEmptyPayload) {
  const std::vector<std::byte> empty;

  EXPECT_EQ(decodeMessage<Heartbeat>(empty), Heartbeat{});
}

/**
 * Given a payload of one byte
 * When it is decoded as a heartbeat
 * Then a TrailingBytesException is raised
 */
TEST(Heartbeat, RefusesAnyByteInThePayload) {
  const auto bytes = byteSequence({0x00});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes), TrailingBytesException);
}

}  // namespace
}  // namespace rtype::network
