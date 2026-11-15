#include <gtest/gtest.h>
#include <vector>
#include "CasinoChannelConstants.hpp"
#include "CasinoChannelFixture.hpp"
#include "CasinoChannelTestConstants.hpp"
#include "CasinoPlace.hpp"
#include "ChipMoves.hpp"
#include "LobbyState.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "ServerSessionTestConstants.hpp"

namespace rtype::client::testing {

using network::CasinoPlace;
using network::RouletteBet;
using network::RouletteBetKind;
using network::RoulettePhase;
using network::TablePlace;

/**
 * Given no session
 * When the player chooses the table
 * Then nothing is sent
 */
TEST_F(CasinoChannelFixture, SendsNothingWithoutASession) {
  casino().chooseTable(TablePlace::AtTable);

  EXPECT_TRUE(sentPlaces().empty());
}

/**
 * Given an accepted session
 * When the player chooses the table, twice
 * Then a single CasinoPlace at the table goes out
 */
TEST_F(CasinoChannelFixture, SendsTheChosenPlaceOnce) {
  connectAndGetAccepted();

  casino().chooseTable(TablePlace::AtTable);
  casino().chooseTable(TablePlace::AtTable);

  EXPECT_EQ(sentPlaces(), std::vector<CasinoPlace>{
                              CasinoPlace{.place = TablePlace::AtTable}});
}

/**
 * Given a place at the table no round shows yet
 * When an interval goes by, then a round lists the player, then another
 * interval goes by
 * Then the place goes out again once, and no more once it is shown
 */
TEST_F(CasinoChannelFixture, SendsThePlaceAgainUntilARoundListsThePlayer) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);

  update(CASINO_MESSAGE_INTERVAL);
  ASSERT_EQ(sentPlaces().size(), 2U);
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Betting));
  update(CASINO_MESSAGE_INTERVAL);

  EXPECT_EQ(sentPlaces().size(), 2U);
}

/**
 * Given a player who went back to the room while the room still shows it at
 * the casino
 * When an interval goes by, then the room shows it not ready, then another
 * Then the place goes out again once, and no more once the room shows it
 */
TEST_F(CasinoChannelFixture, SendsTheRoomAgainWhileTheRoomShowsTheCasino) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);
  room().setLobbyPlayers(
      {network::LobbyPlayer{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                            .place = network::PilotPlace::AtCasino,
                            .nickname = std::string{NICKNAME}}});
  casino().chooseTable(TablePlace::InRoom);

  update(CASINO_MESSAGE_INTERVAL);
  ASSERT_EQ(sentPlaces().back(), CasinoPlace{.place = TablePlace::InRoom});
  const auto sentBefore = sentPlaces().size();
  room().setLobbyPlayers(
      {network::LobbyPlayer{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                            .place = network::PilotPlace::NotReady,
                            .nickname = std::string{NICKNAME}}});
  update(CASINO_MESSAGE_INTERVAL);

  EXPECT_EQ(sentPlaces().size(), sentBefore);
}

/**
 * Given a seated player while the bets of a round are open
 * When it lays a bet, then an interval goes by
 * Then the bets go out for that round at once, and again after the interval
 */
TEST_F(CasinoChannelFixture, SendsTheBetsAgainWhileTheBetsAreOpen) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Betting));
  const std::vector<RouletteBet> bets{
      RouletteBet{.kind = RouletteBetKind::Straight,
                  .firstNumber = BET_NUMBER,
                  .amount = STAKE_OF_A_HUNDRED}};

  casino().placeBets(bets, false);
  ASSERT_EQ(sentBets().size(), 1U);
  update(CASINO_MESSAGE_INTERVAL);

  ASSERT_EQ(sentBets().size(), 2U);
  EXPECT_EQ(sentBets().back(),
            (network::RouletteBets{
                .round = SOME_ROUND, .spinVote = false, .bets = bets}));
}

/**
 * Given bets laid while the bets were open
 * When the wheel turns, then an interval goes by
 * Then the bets are not sent again
 */
TEST_F(CasinoChannelFixture, StopsSendingTheBetsOnceTheWheelTurns) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Betting));
  casino().placeBets({}, true);

  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Spinning));
  update(CASINO_MESSAGE_INTERVAL);

  EXPECT_EQ(sentBets().size(), 1U);
}

/**
 * Given a player who never sat at the table
 * When it lays a bet
 * Then nothing is sent
 */
TEST_F(CasinoChannelFixture, IgnoresBetsOfAPlayerInTheRoom) {
  connectAndGetAccepted();
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Betting));

  casino().placeBets({}, true);

  EXPECT_TRUE(sentBets().empty());
}

/**
 * Given a seated player whose round the server settled with a stake of 100 and
 * a payout of 3600
 * When the spin arrives, a copy of it, then the result
 * Then the chips moved are 100 staked and 3600 won, once, and nothing more on
 * the next call
 */
TEST_F(CasinoChannelFixture, SettlesARoundOnceAcrossItsCopies) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);

  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Spinning));
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Spinning));
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Result));

  EXPECT_EQ(casino().takeChipMoves(), (ChipMoves{.staked = STAKE_OF_A_HUNDRED,
                                                 .won = PAYOUT_OF_A_HUNDRED}));
  EXPECT_EQ(casino().takeChipMoves(), ChipMoves{});
}

/**
 * Given a seated player while the bets of a round are open
 * When the round arrives
 * Then no chip moves: nothing is drawn yet
 */
TEST_F(CasinoChannelFixture, MovesNoChipWhileTheBetsAreOpen) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);

  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Betting));

  EXPECT_EQ(casino().takeChipMoves(), ChipMoves{});
}

/**
 * Given a round kept
 * When the round before it arrives late
 * Then the round kept stays, and the late one moves no chip
 */
TEST_F(CasinoChannelFixture, DropsAnOlderRound) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Betting));

  serverSends(roundListingThePlayer(ROUND_BEFORE, RoulettePhase::Result));

  ASSERT_TRUE(casino().round().has_value());
  EXPECT_EQ(casino().round()->round, SOME_ROUND);
  EXPECT_EQ(casino().takeChipMoves(), ChipMoves{});
}

/**
 * Given a wheel that turns
 * When a copy of the bets of that same round arrives late
 * Then the wheel still turns
 */
TEST_F(CasinoChannelFixture, DropsAnEarlierPhaseOfTheSameRound) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Spinning));

  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Betting));

  ASSERT_TRUE(casino().round().has_value());
  EXPECT_EQ(casino().round()->phase, RoulettePhase::Spinning);
}

/**
 * Given a round kept
 * When the game starts and a frame goes by
 * Then the table is forgotten
 */
TEST_F(CasinoChannelFixture, ForgetsTheTableWhenTheGameStarts) {
  connectAndGetAccepted();
  casino().chooseTable(TablePlace::AtTable);
  serverSends(roundListingThePlayer(SOME_ROUND, RoulettePhase::Betting));

  room().setGameStarted(true);
  update();

  EXPECT_FALSE(casino().round().has_value());
}

}  // namespace rtype::client::testing
