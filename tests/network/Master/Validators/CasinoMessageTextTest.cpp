#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "CasinoMessageTextFixtures.hpp"
#include "CasinoMessageTextTestConstants.hpp"
#include "CasinoRoomState.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteBets.hpp"
#include "RoulettePhase.hpp"
#include "RoundBets.hpp"

using rtype::network::betsReceiptFromText;
using rtype::network::betsReceiptText;
using rtype::network::casinoRoomListingFromText;
using rtype::network::casinoRoomListingText;
using rtype::network::CasinoRoomState;
using rtype::network::casinoRoomStateFromText;
using rtype::network::casinoRoomStateText;
using rtype::network::InvalidMasterMessageException;
using rtype::network::RouletteBetKind;
using rtype::network::rouletteBetKindOf;
using rtype::network::rouletteBetKindText;
using rtype::network::RouletteBets;
using rtype::network::rouletteBetsFromText;
using rtype::network::rouletteBetsText;
using rtype::network::RoulettePhase;
using rtype::network::roulettePhaseOf;
using rtype::network::RoundBets;
using rtype::network::roundBetsFromText;
using rtype::network::roundBetsText;
using rtype::network::testing::drawnRoomState;
using rtype::network::testing::NO_PHASE_TEXT;
using rtype::network::testing::NULL_WINNING_NUMBER;
using rtype::network::testing::ONE_STRAIGHT_BODY;
using rtype::network::testing::oneRoomListing;
using rtype::network::testing::paidRoundBets;
using rtype::network::testing::RESULT_PHASE_TEXT;
using rtype::network::testing::STRAIGHT_AMOUNT;
using rtype::network::testing::STRAIGHT_NUMBER;
using rtype::network::testing::straightBetsBody;
using rtype::network::testing::TOO_LONG_BET_LIST;
using rtype::network::testing::twoBets;
using rtype::network::testing::twoBetsReceipt;
using rtype::network::testing::UNKNOWN_KIND_BODY;
using rtype::network::testing::UNKNOWN_KIND_POINTER;

/**
 * Given the rooms of the casino
 * When they are written, then read back
 * Then the same rooms come back
 */
TEST(CasinoMessageText, TheRoomsReadBack) {
  EXPECT_EQ(casinoRoomListingFromText(casinoRoomListingText(oneRoomListing())),
            oneRoomListing());
}

/**
 * Given the state of a room whose number is drawn, with its history and a
 * paid player
 * When it is written, then read back
 * Then the same state comes back
 */
TEST(CasinoMessageText, TheStateOfARoomReadsBack) {
  EXPECT_EQ(casinoRoomStateFromText(casinoRoomStateText(drawnRoomState())),
            drawnRoomState());
}

/**
 * Given the state of a room whose bets are open
 * When it is written
 * Then its number is written null, and reads back as none
 */
TEST(CasinoMessageText, AnUndrawnNumberIsWrittenNull) {
  CasinoRoomState open = drawnRoomState();
  open.phase = RoulettePhase::Betting;
  open.winningNumber = std::nullopt;

  const std::string text = casinoRoomStateText(open);

  EXPECT_NE(text.find(NULL_WINNING_NUMBER), std::string::npos);
  EXPECT_EQ(casinoRoomStateFromText(text), open);
}

/**
 * Given two bets, their receipt, and the bets of a paid round
 * When each is written, then read back
 * Then each comes back the same
 */
TEST(CasinoMessageText, TheBetsAndTheirAnswersReadBack) {
  EXPECT_EQ(rouletteBetsFromText(rouletteBetsText(twoBets())), twoBets());
  EXPECT_EQ(betsReceiptFromText(betsReceiptText(twoBetsReceipt())),
            twoBetsReceipt());
  EXPECT_EQ(roundBetsFromText(roundBetsText(paidRoundBets())), paidRoundBets());
}

/**
 * Given a body of one straight bet on 17, as a client writes it
 * When the master reads it
 * Then it holds that bet
 */
TEST(CasinoMessageText, ReadsABetByTheNameOfItsKind) {
  const RouletteBets bets = rouletteBetsFromText(ONE_STRAIGHT_BODY);

  ASSERT_EQ(bets.bets.size(), 1U);
  EXPECT_EQ(bets.bets.front().kind, RouletteBetKind::Straight);
  EXPECT_EQ(bets.bets.front().firstNumber, STRAIGHT_NUMBER);
  EXPECT_EQ(bets.bets.front().amount, STRAIGHT_AMOUNT);
}

/**
 * Given a bet of a kind no roulette has
 * When the master reads it
 * Then it refuses the body, naming the kind
 */
TEST(CasinoMessageText, RefusesAKindNoRouletteHas) {
  try {
    static_cast<void>(rouletteBetsFromText(UNKNOWN_KIND_BODY));
    FAIL() << "an unknown kind was read";
  } catch (const InvalidMasterMessageException& refused) {
    EXPECT_NE(std::string{refused.what()}.find(UNKNOWN_KIND_POINTER),
              std::string::npos);
  }
}

/**
 * Given a list of 65 bets, one more than the master reads
 * When the master reads it
 * Then it refuses the body
 */
TEST(CasinoMessageText, RefusesAListLongerThanTheMasterReads) {
  EXPECT_THROW(static_cast<void>(
                   rouletteBetsFromText(straightBetsBody(TOO_LONG_BET_LIST))),
               InvalidMasterMessageException);
  EXPECT_NO_THROW(static_cast<void>(
      rouletteBetsFromText(straightBetsBody(TOO_LONG_BET_LIST - 1))));
}

/**
 * Given every kind of bet and every phase
 * When each is written, then read back
 * Then each comes back the same
 */
TEST(CasinoMessageText, EveryKindAndPhaseReadsBack) {
  for (int kind = 0; kind <= static_cast<int>(RouletteBetKind::High); ++kind) {
    const auto written = static_cast<RouletteBetKind>(kind);
    EXPECT_EQ(rouletteBetKindOf(rouletteBetKindText(written)), written);
  }
  EXPECT_EQ(roulettePhaseOf(RESULT_PHASE_TEXT), RoulettePhase::Result);
  EXPECT_EQ(roulettePhaseOf(NO_PHASE_TEXT), std::nullopt);
}
