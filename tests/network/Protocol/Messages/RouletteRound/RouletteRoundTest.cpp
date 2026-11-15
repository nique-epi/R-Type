#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "RouletteRound.hpp"
#include "RouletteRoundTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given the result of round 0x0304, 17 drawn, and player 1 who staked 500 and
 * is paid 18000
 * When the RouletteRound is encoded
 * Then the payload is round, phase 02, ticks, number 11, one seat, then
 * number 01, stake 000001F4 and payout 00004650
 */
TEST(RouletteRound, EncodesTheRoundThenEachSeat) {
  const RouletteRound round{
      .round = SOME_ROUND_OF_DISTINCT_BYTES,
      .phase = RoulettePhase::Result,
      .ticksLeft = SOME_TICKS_LEFT,
      .winningNumber = DRAWN_SEVENTEEN,
      .seats = {RouletteSeat{.playerNumber = FIRST_SEAT_NUMBER,
                             .stake = STAKE_OF_FIVE_HUNDRED,
                             .payout = STRAIGHT_PAYOUT_OF_FIVE_HUNDRED}}};

  EXPECT_EQ(encodeMessage(round),
            byteSequence({0x03, 0x04, 0x02, 0x05, 0x06, 0x11, 0x01, 0x01, 0x00,
                          0x00, 0x01, 0xF4, 0x00, 0x00, 0x46, 0x50}));
}

/**
 * Given an open table where two players sit, the wheel showing its highest
 * number
 * When the RouletteRound is encoded then decoded
 * Then the decoded message equals the original
 */
TEST(RouletteRound, DecodesWhatItEncoded) {
  const RouletteRound round{
      .round = SOME_ROUND_OF_DISTINCT_BYTES,
      .phase = RoulettePhase::Betting,
      .ticksLeft = SOME_TICKS_LEFT,
      .winningNumber = HIGHEST_ROULETTE_NUMBER,
      .seats = {
          RouletteSeat{.playerNumber = FIRST_SEAT_NUMBER,
                       .stake = STAKE_OF_FIVE_HUNDRED,
                       .payout = 0},
          RouletteSeat{
              .playerNumber = SECOND_SEAT_NUMBER, .stake = 0, .payout = 0}}};

  EXPECT_EQ(decodeMessage<RouletteRound>(encodeMessage(round)), round);
}

/**
 * Given a table where nobody sits
 * When the RouletteRound is encoded then decoded
 * Then the decoded message lists no seat
 */
TEST(RouletteRound, DecodesAnEmptyTable) {
  const RouletteRound empty{.round = 0,
                            .phase = RoulettePhase::Spinning,
                            .ticksLeft = 0,
                            .winningNumber = 0,
                            .seats = {}};

  EXPECT_EQ(decodeMessage<RouletteRound>(encodeMessage(empty)), empty);
}

/**
 * Given a phase byte of 03, past the last phase
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteRound, RefusesAnUnknownPhase) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteRound>(byteSequence(
                   {0x00, 0x00, PHASE_PAST_THE_LAST, 0x00, 0x00, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a winning number of 37, past the wheel
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteRound, RefusesANumberPastTheWheel) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteRound>(
                   byteSequence({0x00, 0x00, 0x02, 0x00, 0x00,
                                 WINNING_NUMBER_PAST_THE_WHEEL, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload announcing five seats, one more than a game has
 * When it is decoded
 * Then it is refused before any seat is read
 */
TEST(RouletteRound, RefusesMoreSeatsThanPlayers) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteRound>(
                   byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05}))),
               TooManyPlayersException);
}

/**
 * Given the same player seated twice
 * When the RouletteRound is encoded
 * Then it is refused
 */
TEST(RouletteRound, RefusesAPlayerSeatedTwice) {
  const RouletteRound twice{
      .round = 0,
      .phase = RoulettePhase::Betting,
      .ticksLeft = 0,
      .winningNumber = 0,
      .seats = {
          RouletteSeat{
              .playerNumber = FIRST_SEAT_NUMBER, .stake = 0, .payout = 0},
          RouletteSeat{
              .playerNumber = FIRST_SEAT_NUMBER, .stake = 0, .payout = 0}}};

  EXPECT_THROW(static_cast<void>(encodeMessage(twice)),
               DuplicatePlayerNumberException);
}

/**
 * Given a payload announcing one seat and cut in its stake
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteRound, RefusesASeatCutShort) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteRound>(byteSequence(
                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00}))),
               BufferUnderflowException);
}

/**
 * Given a payload with a byte after its last seat
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteRound, RefusesBytesAfterTheLastSeat) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteRound>(byteSequence(
                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing
