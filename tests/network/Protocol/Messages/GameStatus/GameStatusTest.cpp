#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "ByteSequence.hpp"
#include "GameStatus.hpp"
#include "GameStatusTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given a game in progress with ACE and BOB, which measured nothing
 * When its status is encoded
 * Then the payload is the state 2, the count 2, each nickname after its size,
 * and a measures byte of 0
 */
TEST(GameStatus, EncodesTheStateThenEachNicknameThenNoMeasure) {
  EXPECT_EQ(encodeMessage(
                GameStatus{.state = GameStatusState::InProgress,
                           .nicknames = {std::string{ACE}, std::string{BOB}},
                           .measures = {}}),
            byteSequence({0x02, 0x02, 0x03, 0x41, 0x43, 0x45, 0x03, 0x42, 0x4F,
                          0x42, 0x00}));
}

/**
 * Given a game with no player that measured only the 95th percentile of its
 * ticks, 0x01020304 microseconds
 * When its status is encoded
 * Then the measures byte holds bit 1 alone, and the measure follows on four
 * bytes, most significant first
 */
TEST(GameStatus, EncodesAMeasureTakenAfterItsBit) {
  EXPECT_EQ(encodeMessage(GameStatus{
                .state = GameStatusState::WaitingForPlayers,
                .nicknames = {},
                .measures = {.tickMedianMicroseconds = std::nullopt,
                             .tickNinetyFifthPercentileMicroseconds =
                                 FOUR_BYTE_MEASURE,
                             .averageRoundTripMicroseconds = std::nullopt}}),
            byteSequence({0x01, 0x00, 0x02, 0x01, 0x02, 0x03, 0x04}));
}

/**
 * Given a status of each state the protocol lists, with no player
 * When each is encoded then decoded
 * Then the decoded status equals the original
 */
TEST(GameStatus, DecodesWhatItEncodedForEveryState) {
  for (const GameStatusState state :
       {GameStatusState::WaitingForPlayers, GameStatusState::InProgress,
        GameStatusState::Finished}) {
    const GameStatus status{.state = state, .nicknames = {}, .measures = {}};

    EXPECT_EQ(decodeMessage<GameStatus>(encodeMessage(status)), status);
  }
}

/**
 * Given a full game of four players that took every measure
 * When its status is encoded then decoded
 * Then the decoded status equals the original
 */
TEST(GameStatus, DecodesFourNicknamesAndEveryMeasure) {
  const GameStatus status{
      .state = GameStatusState::WaitingForPlayers,
      .nicknames = {std::string{ACE}, std::string{BOB}, std::string{CID},
                    std::string{DAN}},
      .measures = {
          .tickMedianMicroseconds = TICK_MEDIAN_MICROSECONDS,
          .tickNinetyFifthPercentileMicroseconds = FOUR_BYTE_MEASURE,
          .averageRoundTripMicroseconds = AVERAGE_ROUND_TRIP_MICROSECONDS}};

  EXPECT_EQ(decodeMessage<GameStatus>(encodeMessage(status)), status);
}

/**
 * Given a status that took the median of its ticks and the mean round trip,
 * not the 95th percentile
 * When it is encoded then decoded
 * Then the 95th percentile stays absent and the two others keep their value
 */
TEST(GameStatus, DecodesTheMeasuresTakenAndLeavesTheOthersAbsent) {
  const GameStatus status{
      .state = GameStatusState::InProgress,
      .nicknames = {std::string{ACE}},
      .measures = {
          .tickMedianMicroseconds = TICK_MEDIAN_MICROSECONDS,
          .tickNinetyFifthPercentileMicroseconds = std::nullopt,
          .averageRoundTripMicroseconds = AVERAGE_ROUND_TRIP_MICROSECONDS}};

  EXPECT_EQ(decodeMessage<GameStatus>(encodeMessage(status)).measures,
            status.measures);
}

/**
 * Given a payload whose state byte is 0, which no state uses
 * When it is decoded
 * Then it is refused
 */
TEST(GameStatus, RefusesAStateByteThatNoStateUses) {
  EXPECT_THROW(static_cast<void>(
                   decodeMessage<GameStatus>(byteSequence({0x00, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload whose state byte is 4, the first one above the last state
 * When it is decoded
 * Then it is refused
 */
TEST(GameStatus, RefusesTheFirstStateByteAboveTheLastState) {
  EXPECT_THROW(static_cast<void>(
                   decodeMessage<GameStatus>(byteSequence({0x04, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload that announces five nicknames and holds none
 * When it is decoded
 * Then it is refused for the count, before any nickname is read
 */
TEST(GameStatus, RefusesMoreNicknamesThanAGameHolds) {
  EXPECT_THROW(static_cast<void>(decodeMessage<GameStatus>(
                   byteSequence({0x01, FIVE_NICKNAMES_BYTE}))),
               TooManyPlayersException);
}

/**
 * Given a status with five nicknames
 * When it is encoded
 * Then it is refused
 */
TEST(GameStatus, RefusesToEncodeMoreNicknamesThanAGameHolds) {
  EXPECT_THROW(
      static_cast<void>(encodeMessage(GameStatus{
          .state = GameStatusState::WaitingForPlayers,
          .nicknames = {std::string{ACE}, std::string{BOB}, std::string{CID},
                        std::string{DAN}, std::string{EVE}},
          .measures = {}})),
      TooManyPlayersException);
}

/**
 * Given a status whose only nickname is 17 bytes long
 * When it is encoded
 * Then it is refused
 */
TEST(GameStatus, RefusesToEncodeANicknameTooLong) {
  EXPECT_THROW(static_cast<void>(encodeMessage(GameStatus{
                   .state = GameStatusState::WaitingForPlayers,
                   .nicknames = {std::string{NICKNAME_OF_SEVENTEEN_BYTES}},
                   .measures = {}})),
               InvalidNicknameException);
}

/**
 * Given a payload whose only nickname is empty
 * When it is decoded
 * Then it is refused
 */
TEST(GameStatus, RefusesAnEmptyNickname) {
  EXPECT_THROW(static_cast<void>(decodeMessage<GameStatus>(
                   byteSequence({0x01, 0x01, 0x00, 0x00}))),
               InvalidNicknameException);
}

/**
 * Given a payload whose measures byte sets bit 3, which no measure uses
 * When it is decoded
 * Then it is refused
 */
TEST(GameStatus, RefusesAMeasureBitNoMeasureUses) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<GameStatus>(byteSequence(
          {0x01, 0x00, UNKNOWN_MEASURE_BIT_BYTE, 0x00, 0x00, 0x00, 0x00}))),
      InvalidFieldValueException);
}

/**
 * Given a payload that stops after its nicknames, with no measures byte
 * When it is decoded
 * Then it is refused as too short
 */
TEST(GameStatus, RefusesAPayloadWithoutItsMeasuresByte) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<GameStatus>(byteSequence({0x01, 0x00}))),
      BufferUnderflowException);
}

/**
 * Given a payload whose measures byte announces the median and that holds
 * only three of its four bytes
 * When it is decoded
 * Then it is refused as too short
 */
TEST(GameStatus, RefusesAMeasureCutShort) {
  EXPECT_THROW(static_cast<void>(decodeMessage<GameStatus>(
                   byteSequence({0x01, 0x00, 0x01, 0x00, 0x00, 0x07}))),
               BufferUnderflowException);
}

/**
 * Given a payload with no player and no measure, then one more byte
 * When it is decoded
 * Then it is refused, since a byte follows the last measure
 */
TEST(GameStatus, RefusesAByteAfterTheMeasures) {
  EXPECT_THROW(static_cast<void>(decodeMessage<GameStatus>(
                   byteSequence({0x01, 0x00, 0x00, 0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing
