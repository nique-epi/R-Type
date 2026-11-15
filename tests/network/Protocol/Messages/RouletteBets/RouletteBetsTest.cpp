#include <gtest/gtest.h>
#include <vector>
#include "ByteSequence.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "RouletteBets.hpp"
#include "RouletteBetsTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given round 0x0102, a vote to spin, and 500 chips straight on 17
 * When the RouletteBets is encoded
 * Then the payload is the round, the vote, one bet, then kind 00, number 11
 * and amount 000001F4
 */
TEST(RouletteBets, EncodesTheRoundTheVoteThenEachBet) {
  const RouletteBets bets{
      .round = SOME_ROULETTE_ROUND,
      .spinVote = true,
      .bets = {RouletteBet{.kind = RouletteBetKind::Straight,
                           .firstNumber = SEVENTEEN,
                           .amount = FIVE_HUNDRED_CHIPS}}};

  EXPECT_EQ(encodeMessage(bets), byteSequence({0x01, 0x02, 0x01, 0x01, 0x00,
                                               0x11, 0x00, 0x00, 0x01, 0xF4}));
}

/**
 * Given a player with no chip on the cloth who does not vote
 * When the RouletteBets is encoded then decoded
 * Then the decoded message lists no bet
 */
TEST(RouletteBets, DecodesAnEmptyCloth) {
  const RouletteBets empty{
      .round = SOME_ROULETTE_ROUND, .spinVote = false, .bets = {}};

  EXPECT_EQ(decodeMessage<RouletteBets>(encodeMessage(empty)), empty);
}

/**
 * Given MAXIMUM_ROULETTE_BETS bets of one chip on the highest number, the
 * last kind of bet
 * When the RouletteBets is encoded then decoded
 * Then the decoded message equals the original
 */
TEST(RouletteBets, DecodesAFullClothAtTheBounds) {
  const RouletteBets full{
      .round = SOME_ROULETTE_ROUND,
      .spinVote = true,
      .bets = std::vector<RouletteBet>(
          MAXIMUM_ROULETTE_BETS,
          RouletteBet{.kind = RouletteBetKind::High,
                      .firstNumber = HIGHEST_ROULETTE_NUMBER,
                      .amount = ONE_CHIP})};

  EXPECT_EQ(decodeMessage<RouletteBets>(encodeMessage(full)), full);
}

/**
 * Given one bet more than MAXIMUM_ROULETTE_BETS
 * When the RouletteBets is encoded
 * Then it is refused
 */
TEST(RouletteBets, RefusesToEncodeOneBetTooMany) {
  const RouletteBets tooMany{
      .round = SOME_ROULETTE_ROUND,
      .spinVote = false,
      .bets = std::vector<RouletteBet>(MAXIMUM_ROULETTE_BETS + 1,
                                       RouletteBet{.kind = RouletteBetKind::Red,
                                                   .firstNumber = 0,
                                                   .amount = ONE_CHIP})};

  EXPECT_THROW(static_cast<void>(encodeMessage(tooMany)),
               TooManyRouletteBetsException);
}

/**
 * Given a payload announcing 33 bets (0x21) and holding none
 * When it is decoded
 * Then it is refused for the count, before any bet is read
 */
TEST(RouletteBets, RefusesACountAboveTheMaximumBeforeReading) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteBets>(
                   byteSequence({0x01, 0x02, 0x00, 0x21}))),
               TooManyRouletteBetsException);
}

/**
 * Given a bet of kind 14, past the last kind
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteBets, RefusesAnUnknownKind) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteBets>(
                   byteSequence({0x01, 0x02, 0x00, 0x01, KIND_PAST_THE_LAST,
                                 0x00, 0x00, 0x00, 0x00, 0x01}))),
               InvalidFieldValueException);
}

/**
 * Given a straight bet on 37, past the highest number of the wheel
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteBets, RefusesANumberPastTheWheel) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteBets>(byteSequence(
                   {0x01, 0x02, 0x00, 0x01, 0x00, NUMBER_PAST_THE_WHEEL, 0x00,
                    0x00, 0x00, 0x01}))),
               InvalidFieldValueException);
}

/**
 * Given a straight bet of no chip
 * When it is encoded
 * Then it is refused
 */
TEST(RouletteBets, RefusesABetOfNoChip) {
  const RouletteBets nothing{
      .round = SOME_ROULETTE_ROUND,
      .spinVote = false,
      .bets = {RouletteBet{.kind = RouletteBetKind::Straight,
                           .firstNumber = SEVENTEEN,
                           .amount = 0}}};

  EXPECT_THROW(static_cast<void>(encodeMessage(nothing)),
               InvalidFieldValueException);
}

/**
 * Given a received straight bet of no chip
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteBets, RefusesAReceivedBetOfNoChip) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<RouletteBets>(byteSequence(
          {0x01, 0x02, 0x00, 0x01, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00}))),
      InvalidFieldValueException);
}

/**
 * Given a vote byte of 02
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteBets, RefusesAVoteThatIsNoBoolean) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteBets>(
                   byteSequence({0x01, 0x02, 0x02, 0x00}))),
               InvalidBoolException);
}

/**
 * Given a payload announcing one bet and cut in the middle of its amount
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteBets, RefusesABetCutShort) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteBets>(
                   byteSequence({0x01, 0x02, 0x00, 0x01, 0x00, 0x11, 0x00}))),
               BufferUnderflowException);
}

/**
 * Given a payload with a byte after its last bet
 * When it is decoded
 * Then it is refused
 */
TEST(RouletteBets, RefusesBytesAfterTheLastBet) {
  EXPECT_THROW(static_cast<void>(decodeMessage<RouletteBets>(
                   byteSequence({0x01, 0x02, 0x00, 0x00, 0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing
