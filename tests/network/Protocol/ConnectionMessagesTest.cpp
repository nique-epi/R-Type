#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionMessagesFixtures.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "GameRules.hpp"
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
using testing::FIRST_NAMED_GAME;
using testing::FIRST_PLAYER_JITTER;
using testing::FIRST_PLAYER_ROUND_TRIP;
using testing::FIVE_LIVES;
using testing::FOUR_ROCKETS;
using testing::GAME_KIND_ABOVE_THE_LAST;
using testing::GAME_KIND_BELOW_THE_FIRST;
using testing::hardRulesWithFriendlyFire;
using testing::HEARTBEAT_SENT_AT;
using testing::HEARTBEAT_TICK;
using testing::LAST_NAMED_GAME;
using testing::newGameRequestWith;
using testing::NICKNAME_WITH_LINE_FEED;
using testing::NO_NAMED_GAME;
using testing::ONE_BYTE_NICKNAME;
using testing::REFUSAL_REASON_ABOVE_THE_LAST;
using testing::requestFor;
using testing::SECOND_PLAYER_JITTER;
using testing::SECOND_PLAYER_NUMBER;
using testing::SECOND_PLAYER_ROUND_TRIP;
using testing::SEVENTEEN_BYTE_NICKNAME;
using testing::SIXTEEN_BYTE_NICKNAME;
using testing::SMALLEST_REQUEST_DATAGRAM_SIZE_ON_THE_PAGE;
using testing::smallestRequest;
using testing::SOME_COOKIE;
using testing::writeAnyGameChoice;
using testing::writeRequestFor;

/**
 * Given a request with a cookie and the nickname "A", for any game
 * When it is encoded
 * Then the payload is the cookie on eight bytes, the length 1 and the letter
 * A, then the kind 1 and the number 0 on two bytes
 */
TEST(ConnectionRequest, EncodesTheCookieThenTheNicknameWithItsLength) {
  const auto expected = byteSequence({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x01, 0x41, 0x01, 0x00, 0x00});

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
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x02, 0xC3, 0xA9, 0x01, 0x00, 0x00});

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
  const auto bytes = byteSequence(
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00});

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
  writeAnyGameChoice(writer);

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
  writeAnyGameChoice(writer);

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(writer.bytes()),
               InvalidNicknameException);
}

/**
 * Given a payload whose nickname is the overlong form C0 80
 * When it is decoded
 * Then an InvalidNicknameException is raised
 */
TEST(ConnectionRequest, RefusesToReadANicknameThatIsNotUtf8) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x02, 0xC0, 0x80, 0x01, 0x00, 0x00});

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
TEST(ConnectionRequest, RefusesBytesAfterTheGame) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x01, 0x41, 0x01, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               TrailingBytesException);
}

/**
 * Given a request for any game
 * When it is encoded then decoded
 * Then the decoded request asks for any game
 */
TEST(ConnectionRequest, CarriesAChoiceOfAnyGame) {
  const ConnectionRequest request = requestFor(ANY_GAME_CHOICE);

  EXPECT_EQ(decodeMessage<ConnectionRequest>(encodeMessage(request)), request);
}

/**
 * Given a request for a new game
 * When it is encoded then decoded
 * Then the decoded request asks for a new game
 */
TEST(ConnectionRequest, CarriesAChoiceOfANewGame) {
  const ConnectionRequest request = requestFor(NEW_GAME_CHOICE);

  EXPECT_EQ(decodeMessage<ConnectionRequest>(encodeMessage(request)), request);
}

/**
 * Given a request for game 1, the first a request may name
 * When it is encoded then decoded
 * Then the decoded request asks for game 1
 */
TEST(ConnectionRequest, CarriesTheFirstGameItMayName) {
  const ConnectionRequest request = requestFor(GameChoice{
      .kind = GameChoiceKind::ExistingGame, .number = FIRST_NAMED_GAME});

  EXPECT_EQ(decodeMessage<ConnectionRequest>(encodeMessage(request)), request);
}

/**
 * Given a request for game 65535, the last a request may name
 * When it is encoded then decoded
 * Then the decoded request asks for game 65535
 */
TEST(ConnectionRequest, CarriesTheLastGameItMayName) {
  const ConnectionRequest request = requestFor(GameChoice{
      .kind = GameChoiceKind::ExistingGame, .number = LAST_NAMED_GAME});

  EXPECT_EQ(decodeMessage<ConnectionRequest>(encodeMessage(request)), request);
}

/**
 * Given a request for game 0x0102
 * When it is encoded
 * Then the payload ends with the kind 2, then the number on two bytes, most
 * significant first
 */
TEST(ConnectionRequest, EncodesTheKindThenTheNumberOfTheGame) {
  const auto expected = byteSequence({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x01, 0x41, 0x02, 0x01, 0x02});

  EXPECT_EQ(encodeMessage(requestFor(GameChoice{
                .kind = GameChoiceKind::ExistingGame, .number = 0x0102})),
            expected);
}

/**
 * Given a request for a new game in "hard", with five lives and friendly fire
 * When it is encoded
 * Then the payload ends with the kind 3, the number 0 on two bytes, the length
 * 4 and "hard", then the lives 5 and the friendly fire 1
 */
TEST(ConnectionRequest, EncodesTheRulesAfterTheNumberOfANewGame) {
  const auto expected = byteSequence({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x01, 0x41, 0x03, 0x00, 0x00, 0x04,
                                      0x68, 0x61, 0x72, 0x64, 0x05, 0x01});

  EXPECT_EQ(encodeMessage(newGameRequestWith(hardRulesWithFriendlyFire())),
            expected);
}

/**
 * Given a request for a new game whose rules differ from the default ones on
 * every field
 * When it is encoded then decoded
 * Then the decoded request carries the same rules
 */
TEST(ConnectionRequest, CarriesTheRulesOfANewGame) {
  const ConnectionRequest request =
      newGameRequestWith(hardRulesWithFriendlyFire());

  EXPECT_EQ(decodeMessage<ConnectionRequest>(encodeMessage(request)), request);
}

/**
 * Given a request for a new game whose difficulty is 16 bytes long, the
 * longest a nickname may be
 * When it is encoded then decoded
 * Then the decoded request carries that difficulty
 */
TEST(ConnectionRequest, CarriesADifficultyOfSixteenBytes) {
  const ConnectionRequest request = newGameRequestWith(
      GameRules{.difficulty = std::string{SIXTEEN_BYTE_NICKNAME},
                .lives = FIVE_LIVES,
                .friendlyFire = false});

  EXPECT_EQ(decodeMessage<ConnectionRequest>(encodeMessage(request)), request);
}

/**
 * Given a request for a new game whose difficulty is 17 bytes long
 * When it is encoded
 * Then an InvalidDifficultyException is raised
 */
TEST(ConnectionRequest, RefusesToWriteADifficultyOfSeventeenBytes) {
  const ConnectionRequest request = newGameRequestWith(
      GameRules{.difficulty = std::string{SEVENTEEN_BYTE_NICKNAME},
                .lives = FIVE_LIVES,
                .friendlyFire = false});

  EXPECT_THROW((void)encodeMessage(request), InvalidDifficultyException);
}

/**
 * Given a request for a new game that carries no rules
 * When it is encoded
 * Then an InvalidFieldValueException is raised: a new game always has rules
 */
TEST(ConnectionRequest, RefusesToWriteANewGameWithoutRules) {
  ConnectionRequest request = requestFor(NEW_GAME_CHOICE);
  request.rules = std::nullopt;

  EXPECT_THROW((void)encodeMessage(request), InvalidFieldValueException);
}

/**
 * Given a request for any game that carries rules
 * When it is encoded
 * Then an InvalidFieldValueException is raised: only a new game has rules
 */
TEST(ConnectionRequest, RefusesToWriteRulesWithAnyGame) {
  ConnectionRequest request = requestFor(ANY_GAME_CHOICE);
  request.rules = hardRulesWithFriendlyFire();

  EXPECT_THROW((void)encodeMessage(request), InvalidFieldValueException);
}

/**
 * Given a payload for a new game whose friendly fire is the byte 2
 * When it is decoded
 * Then an InvalidBoolException is raised: friendly fire is 0 or 1
 */
TEST(ConnectionRequest, RefusesToReadAFriendlyFireOfTwo) {
  const auto bytes = byteSequence({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                   0x08, 0x01, 0x41, 0x03, 0x00, 0x00, 0x04,
                                   0x68, 0x61, 0x72, 0x64, 0x05, 0x02});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               InvalidBoolException);
}

/**
 * Given a payload for a new game whose difficulty is empty
 * When it is decoded
 * Then an InvalidDifficultyException is raised
 */
TEST(ConnectionRequest, RefusesToReadAnEmptyDifficulty) {
  const auto bytes =
      byteSequence({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x41,
                    0x03, 0x00, 0x00, 0x00, 0x03, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               InvalidDifficultyException);
}

/**
 * Given a payload for a new game that ends after the number of the game
 * When it is decoded
 * Then a BufferUnderflowException is raised: the rules are missing
 */
TEST(ConnectionRequest, RefusesToReadANewGameWithoutItsRules) {
  const auto bytes = byteSequence({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                   0x08, 0x01, 0x41, 0x03, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               BufferUnderflowException);
}

/**
 * Given a payload whose kind of game is 0, just below the kinds of the
 * protocol page
 * When it is decoded
 * Then an InvalidFieldValueException is raised
 */
TEST(ConnectionRequest, RefusesToReadAKindBelowTheFirst) {
  ByteWriter writer;
  // NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange)
  const auto unlisted = static_cast<GameChoiceKind>(GAME_KIND_BELOW_THE_FIRST);
  // NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange)
  writeRequestFor(writer,
                  GameChoice{.kind = unlisted, .number = NO_NAMED_GAME});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(writer.bytes()),
               InvalidFieldValueException);
}

/**
 * Given a payload whose kind of game is 4, just above the kinds of the
 * protocol page
 * When it is decoded
 * Then an InvalidFieldValueException is raised
 */
TEST(ConnectionRequest, RefusesToReadAKindAboveTheLast) {
  ByteWriter writer;
  // NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange)
  const auto unlisted = static_cast<GameChoiceKind>(GAME_KIND_ABOVE_THE_LAST);
  // NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange)
  writeRequestFor(writer,
                  GameChoice{.kind = unlisted, .number = NO_NAMED_GAME});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(writer.bytes()),
               InvalidFieldValueException);
}

/**
 * Given a payload that asks for any game and names game 1
 * When it is decoded
 * Then an InvalidFieldValueException is raised: only a choice of an existing
 * game carries a number
 */
TEST(ConnectionRequest, RefusesToReadANumberWithAnyGame) {
  ByteWriter writer;
  writeRequestFor(writer, GameChoice{.kind = GameChoiceKind::AnyGame,
                                     .number = FIRST_NAMED_GAME});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(writer.bytes()),
               InvalidFieldValueException);
}

/**
 * Given a payload that asks for a new game and names game 1
 * When it is decoded
 * Then an InvalidFieldValueException is raised
 */
TEST(ConnectionRequest, RefusesToReadANumberWithANewGame) {
  ByteWriter writer;
  writeRequestFor(writer, GameChoice{.kind = GameChoiceKind::NewGame,
                                     .number = FIRST_NAMED_GAME});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(writer.bytes()),
               InvalidFieldValueException);
}

/**
 * Given a payload that asks for the existing game 0
 * When it is decoded
 * Then an InvalidFieldValueException is raised: the first game is 1
 */
TEST(ConnectionRequest, RefusesToReadExistingGameZero) {
  ByteWriter writer;
  writeRequestFor(writer, GameChoice{.kind = GameChoiceKind::ExistingGame,
                                     .number = NO_NAMED_GAME});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(writer.bytes()),
               InvalidFieldValueException);
}

/**
 * Given a request for the existing game 0
 * When writing it is refused
 * Then nothing was written, not even the cookie that comes first
 */
TEST(ConnectionRequest, RefusedChoiceWritesNothing) {
  ByteWriter writer;

  EXPECT_THROW(encodePayload(writer, requestFor(GameChoice{
                                         .kind = GameChoiceKind::ExistingGame,
                                         .number = NO_NAMED_GAME})),
               InvalidFieldValueException);

  EXPECT_TRUE(writer.bytes().empty());
}

/**
 * Given a request whose kind of game is 4, outside the protocol page
 * When writing it is refused
 * Then nothing was written
 */
TEST(ConnectionRequest, RefusedKindWritesNothing) {
  ByteWriter writer;
  // NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange)
  const auto unlisted = static_cast<GameChoiceKind>(GAME_KIND_ABOVE_THE_LAST);
  // NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange)

  EXPECT_THROW(
      encodePayload(writer, requestFor(GameChoice{.kind = unlisted,
                                                  .number = NO_NAMED_GAME})),
      InvalidFieldValueException);

  EXPECT_TRUE(writer.bytes().empty());
}

/**
 * Given a request whose number of game lacks its last byte
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(ConnectionRequest, RefusesARequestCutInTheNumberOfTheGame) {
  const auto bytes = byteSequence(
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x41, 0x02, 0x01});

  EXPECT_THROW((void)decodeMessage<ConnectionRequest>(bytes),
               BufferUnderflowException);
}

/**
 * Given a request with a one-byte nickname, the smallest there is
 * When its datagram is encoded
 * Then it holds the 38 bytes the protocol page gives
 */
TEST(ConnectionRequest, SmallestRequestDatagramHoldsThirtyEightBytes) {
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
 * gives a sender that did not prove its address, that is a request without a
 * valid cookie
 * When their datagrams are encoded
 * Then no answer is larger than the request, so the server cannot be used to
 * send a victim more bytes than the forger sent
 */
TEST(ConnectionChallenge, NoUnprovenAnswerIsLargerThanTheSmallestRequest) {
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
 * Given the payloads 01 to 06
 * When each is decoded
 * Then they give GameFull, GameInProgress, IncompatibleVersion,
 * NicknameTaken, GameNotFound and ServerFull
 */
TEST(ConnectionRefused, DecodesEveryReasonOfTheProtocolPage) {
  const auto gameFull = byteSequence({0x01});
  const auto gameInProgress = byteSequence({0x02});
  const auto incompatibleVersion = byteSequence({0x03});
  const auto nicknameTaken = byteSequence({0x04});
  const auto gameNotFound = byteSequence({0x05});
  const auto serverFull = byteSequence({0x06});

  EXPECT_EQ(decodeMessage<ConnectionRefused>(gameFull).reason,
            RefusalReason::GameFull);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(gameInProgress).reason,
            RefusalReason::GameInProgress);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(incompatibleVersion).reason,
            RefusalReason::IncompatibleVersion);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(nicknameTaken).reason,
            RefusalReason::NicknameTaken);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(gameNotFound).reason,
            RefusalReason::GameNotFound);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(serverFull).reason,
            RefusalReason::ServerFull);
}

/**
 * Given the payloads 07 and 08
 * When each is decoded
 * Then they give UnknownDifficulty and LivesOutOfRange, the refusals of the
 * rules of a new game
 */
TEST(ConnectionRefused, DecodesTheRefusalsOfTheRulesOfANewGame) {
  const auto unknownDifficulty = byteSequence({0x07});
  const auto livesOutOfRange = byteSequence({0x08});

  EXPECT_EQ(decodeMessage<ConnectionRefused>(unknownDifficulty).reason,
            RefusalReason::UnknownDifficulty);
  EXPECT_EQ(decodeMessage<ConnectionRefused>(livesOutOfRange).reason,
            RefusalReason::LivesOutOfRange);
}

/**
 * Given the payloads 00 and 09, just outside the reasons of the protocol page
 * When each is decoded
 * Then each is refused
 */
TEST(ConnectionRefused, RefusesToReadAReasonOutsideTheList) {
  const auto belowTheFirst = byteSequence({0x00});
  const auto aboveTheLast = byteSequence({0x09});

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
