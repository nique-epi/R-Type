#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionMessagesFixtures.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Heartbeat.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolTestConstants.hpp"
#include "RoundTrips.hpp"

namespace rtype::network {

using testing::byteSequence;
using testing::CHALLENGE_DATAGRAM_SIZE_ON_THE_PAGE;
using testing::ECHO_HELD_FOR;
using testing::ECHOED_SENT_AT;
using testing::FIRST_PLAYER_JITTER;
using testing::FIRST_PLAYER_ROUND_TRIP;
using testing::FOUR_ROCKETS;
using testing::HEARTBEAT_SENT_AT;
using testing::HEARTBEAT_TICK;
using testing::LAST_PLAYER_NUMBER;
using testing::NICKNAME_WITH_LINE_FEED;
using testing::ONE_BYTE_NICKNAME;
using testing::PLAYER_NUMBER_ABOVE_THE_LAST;
using testing::REFUSAL_REASON_ABOVE_THE_LAST;
using testing::SECOND_PLAYER_JITTER;
using testing::SECOND_PLAYER_NUMBER;
using testing::SECOND_PLAYER_ROUND_TRIP;
using testing::SEVENTEEN_BYTE_NICKNAME;
using testing::SMALLEST_REQUEST_DATAGRAM_SIZE_ON_THE_PAGE;
using testing::smallestRequest;
using testing::SOME_COOKIE;

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
 * Given a payload whose nickname holds a line feed
 * When it is decoded
 * Then an InvalidNicknameException is raised
 */
TEST(ConnectionRequest, RefusesToReadANicknameHoldingALineFeed) {
  ByteWriter writer;
  writer.writeUint64(SOME_COOKIE);
  writer.writeString(NICKNAME_WITH_LINE_FEED);

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
 * Given the payloads 01, 02, 03 and 04
 * When each is decoded
 * Then they give GameFull, GameInProgress, IncompatibleVersion and
 * NicknameTaken
 */
TEST(ConnectionRefused, DecodesEveryReasonOfTheProtocolPage) {
  const auto gameFull = byteSequence({0x01});
  const auto gameInProgress = byteSequence({0x02});
  const auto incompatibleVersion = byteSequence({0x03});
  const auto nicknameTaken = byteSequence({0x04});

  EXPECT_EQ(decodeMessage<ConnectionRefused>(gameFull).reason,
            RefusalReason::GameFull);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(gameInProgress).reason,
            RefusalReason::GameInProgress);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(incompatibleVersion).reason,
            RefusalReason::IncompatibleVersion);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(nicknameTaken).reason,
            RefusalReason::NicknameTaken);
}

/**
 * Given the payloads 00 and 05, just outside the reasons of the protocol page
 * When each is decoded
 * Then each is refused
 */
TEST(ConnectionRefused, RefusesToReadAReasonOutsideTheList) {
  const auto belowTheFirst = byteSequence({0x00});
  const auto aboveTheLast = byteSequence({0x05});

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
 * Given a heartbeat sent at 7 µs that echoes a heartbeat sent at 5 µs, held
 * 3000 µs, and carries no tick
 * When it is encoded
 * Then the payload is its time on four bytes, then 1, then the echoed time
 * and the time held, four bytes each, then 0 and four bytes of 0
 */
TEST(Heartbeat, EncodesItsTimeThenItsEcho) {
  const auto expected =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00,
                    0x00, 0x0B, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_EQ(
      encodeMessage(Heartbeat{.sentAt = HEARTBEAT_SENT_AT,
                              .echo = HeartbeatEcho{.sentAt = ECHOED_SENT_AT,
                                                    .heldFor = ECHO_HELD_FOR},
                              .tick = std::nullopt}),
      expected);
}

/**
 * Given a heartbeat sent at 7 µs that echoes nothing and carries no tick
 * When it is encoded
 * Then the payload is its time, then 0, then eight bytes of 0, then 0 and
 * four bytes of 0, so it has the size of a heartbeat with an echo and a tick
 */
TEST(Heartbeat, EncodesAMissingEchoAsZeros) {
  const auto expected =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_EQ(encodeMessage(Heartbeat{.sentAt = HEARTBEAT_SENT_AT,
                                    .echo = std::nullopt,
                                    .tick = std::nullopt}),
            expected);
}

/**
 * Given a heartbeat sent at 7 µs that echoes nothing and carries tick 600
 * When it is encoded
 * Then the payload is its time, then 0 and eight bytes of 0 for the echo, then
 * 1 and the tick on four bytes
 */
TEST(Heartbeat, EncodesTheTickItCarriesAfterTheEcho) {
  const auto expected =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x58});

  EXPECT_EQ(encodeMessage(Heartbeat{.sentAt = HEARTBEAT_SENT_AT,
                                    .echo = std::nullopt,
                                    .tick = HEARTBEAT_TICK}),
            expected);
}

/**
 * Given a heartbeat with an echo and a tick
 * When it is encoded then decoded
 * Then the same heartbeat comes back
 */
TEST(Heartbeat, ReadsAHeartbeatWithAnEchoAsItWasWritten) {
  const Heartbeat heartbeat{
      .sentAt = HEARTBEAT_SENT_AT,
      .echo = HeartbeatEcho{.sentAt = ECHOED_SENT_AT, .heldFor = ECHO_HELD_FOR},
      .tick = HEARTBEAT_TICK};

  EXPECT_EQ(decodeMessage<Heartbeat>(encodeMessage(heartbeat)), heartbeat);
}

/**
 * Given a heartbeat without an echo and without a tick
 * When it is encoded then decoded
 * Then the same heartbeat comes back, still without either
 */
TEST(Heartbeat, ReadsAHeartbeatWithoutAnEchoAsItWasWritten) {
  const Heartbeat heartbeat{
      .sentAt = HEARTBEAT_SENT_AT, .echo = std::nullopt, .tick = std::nullopt};

  EXPECT_EQ(decodeMessage<Heartbeat>(encodeMessage(heartbeat)), heartbeat);
}

/**
 * Given a heartbeat that carries tick 0, the tick of a game that has just
 * started
 * When it is encoded then decoded
 * Then it comes back with tick 0, not without a tick
 */
TEST(Heartbeat, ReadsATickOfZeroAsATick) {
  const Heartbeat heartbeat{
      .sentAt = HEARTBEAT_SENT_AT, .echo = std::nullopt, .tick = 0};

  EXPECT_EQ(decodeMessage<Heartbeat>(encodeMessage(heartbeat)), heartbeat);
}

/**
 * Given a payload that says no echo follows, then an echoed time of 1
 * When it is decoded as a heartbeat
 * Then an InvalidFieldValueException is raised: a heartbeat has one encoding
 */
TEST(Heartbeat, RefusesAnEchoedTimeBehindAMissingEcho) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a payload that says no echo follows, then a time held of 1
 * When it is decoded as a heartbeat
 * Then an InvalidFieldValueException is raised
 */
TEST(Heartbeat, RefusesATimeHeldBehindAMissingEcho) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a payload that says no tick follows, then a tick of 1
 * When it is decoded as a heartbeat
 * Then an InvalidFieldValueException is raised: a heartbeat has one encoding
 */
TEST(Heartbeat, RefusesATickBehindAMissingTick) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a payload whose byte that says whether an echo follows is 2
 * When it is decoded as a heartbeat
 * Then an InvalidBoolException is raised
 */
TEST(Heartbeat, RefusesAnEchoFlagThatIsNeitherTrueNorFalse) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes), InvalidBoolException);
}

/**
 * Given a payload whose byte that says whether a tick follows is 2
 * When it is decoded as a heartbeat
 * Then an InvalidBoolException is raised
 */
TEST(Heartbeat, RefusesATickFlagThatIsNeitherTrueNorFalse) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes), InvalidBoolException);
}

/**
 * Given a payload of 17 bytes, one fewer than a heartbeat
 * When it is decoded as a heartbeat
 * Then a BufferUnderflowException is raised
 */
TEST(Heartbeat, RefusesAPayloadCutShort) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00,
                    0x00, 0x0B, 0xB8, 0x01, 0x00, 0x00, 0x02});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes), BufferUnderflowException);
}

/**
 * Given the 13 bytes a build that sends the echo but no tick writes
 * When it is decoded as a heartbeat
 * Then a BufferUnderflowException is raised, so the dispatcher skips it
 */
TEST(Heartbeat, RefusesThePayloadOfABuildBeforeTheTick) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x07, 0x01, 0x00, 0x00,
                                   0x00, 0x05, 0x00, 0x00, 0x0B, 0xB8});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes), BufferUnderflowException);
}

/**
 * Given the empty payload a build older than the echo sends
 * When it is decoded as a heartbeat
 * Then a BufferUnderflowException is raised, so the dispatcher skips it
 */
TEST(Heartbeat, RefusesTheEmptyPayloadOfAnOlderBuild) {
  const std::vector<std::byte> empty;

  EXPECT_THROW((void)decodeMessage<Heartbeat>(empty), BufferUnderflowException);
}

/**
 * Given a heartbeat followed by one more byte
 * When it is decoded as a heartbeat
 * Then a TrailingBytesException is raised
 */
TEST(Heartbeat, RefusesAByteAfterTheTick) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00,
                    0x00, 0x0B, 0xB8, 0x01, 0x00, 0x00, 0x02, 0x58, 0x00});

  EXPECT_THROW((void)decodeMessage<Heartbeat>(bytes), TrailingBytesException);
}

/**
 * Given the round trips of players 1 and 2: 42000 µs with a jitter of 3200 µs,
 * and 80000 µs with a jitter of 12500 µs
 * When they are encoded
 * Then the payload is the number of players, then each one: its number, its
 * round trip and its jitter on four bytes each
 */
TEST(RoundTrips, EncodesTheNumberThenEachPlayer) {
  const auto expected =
      byteSequence({0x02, 0x01, 0x00, 0x00, 0xA4, 0x10, 0x00, 0x00, 0x0C, 0x80,
                    0x02, 0x00, 0x01, 0x38, 0x80, 0x00, 0x00, 0x30, 0xD4});

  EXPECT_EQ(encodeMessage(
                RoundTrips{.players = {{.playerNumber = FIRST_PLAYER_NUMBER,
                                        .roundTrip = FIRST_PLAYER_ROUND_TRIP,
                                        .jitter = FIRST_PLAYER_JITTER},
                                       {.playerNumber = SECOND_PLAYER_NUMBER,
                                        .roundTrip = SECOND_PLAYER_ROUND_TRIP,
                                        .jitter = SECOND_PLAYER_JITTER}}}),
            expected);
}

/**
 * Given round trips of no player, of one, and of the most players a game
 * holds, each with its own round trip and jitter
 * When each is encoded then decoded
 * Then the same round trips come back
 */
TEST(RoundTrips, ReadsWhatItWrites) {
  RoundTrips everyPlayer;
  for (std::uint8_t number = FIRST_PLAYER_NUMBER; number <= MAXIMUM_PLAYERS;
       ++number) {
    everyPlayer.players.push_back(
        PlayerRoundTrip{.playerNumber = number,
                        .roundTrip = FIRST_PLAYER_ROUND_TRIP * number,
                        .jitter = FIRST_PLAYER_JITTER * number});
  }
  const std::vector<RoundTrips> written{
      RoundTrips{.players = {}},
      RoundTrips{.players = {{.playerNumber = FIRST_PLAYER_NUMBER,
                              .roundTrip = FIRST_PLAYER_ROUND_TRIP,
                              .jitter = FIRST_PLAYER_JITTER}}},
      everyPlayer};

  for (const RoundTrips& roundTrips : written) {
    EXPECT_EQ(decodeMessage<RoundTrips>(encodeMessage(roundTrips)), roundTrips);
  }
}

/**
 * Given a payload that announces five players
 * When it is decoded as round trips
 * Then a TooManyPlayersException is raised before any player is read
 */
TEST(RoundTrips, RefusesFivePlayersBeforeReadingThem) {
  const auto bytes = byteSequence({0x05});

  EXPECT_THROW((void)decodeMessage<RoundTrips>(bytes), TooManyPlayersException);
}

/**
 * Given a payload that lists player 1 twice
 * When it is decoded as round trips
 * Then a DuplicatePlayerNumberException is raised
 */
TEST(RoundTrips, RefusesTheSameNumberTwice) {
  const auto bytes =
      byteSequence({0x02, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
                    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01});

  EXPECT_THROW((void)decodeMessage<RoundTrips>(bytes),
               DuplicatePlayerNumberException);
}

/**
 * Given a payload that lists player 0
 * When it is decoded as round trips
 * Then an InvalidFieldValueException is raised: players are numbered from 1
 */
TEST(RoundTrips, RefusesPlayerNumberZero) {
  const auto bytes = byteSequence(
      {0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01});

  EXPECT_THROW((void)decodeMessage<RoundTrips>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a payload that announces one player and holds eight of its nine
 * bytes
 * When it is decoded as round trips
 * Then a BufferUnderflowException is raised
 */
TEST(RoundTrips, RefusesAPlayerCutShort) {
  const auto bytes =
      byteSequence({0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<RoundTrips>(bytes),
               BufferUnderflowException);
}

/**
 * Given a payload that holds one player, then one more byte
 * When it is decoded as round trips
 * Then a TrailingBytesException is raised
 */
TEST(RoundTrips, RefusesAByteAfterTheLastPlayer) {
  const auto bytes = byteSequence(
      {0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<RoundTrips>(bytes), TrailingBytesException);
}

}  // namespace rtype::network
