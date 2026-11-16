#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include <string>
#include "BetsReading.hpp"
#include "CasinoChannelFixture.hpp"
#include "CasinoChannelTestConstants.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "OwnBetsReading.hpp"
#include "RoomRound.hpp"
#include "RoulettePhase.hpp"
#include "RoundBets.hpp"

using rtype::client::BetsReading;
using rtype::client::CasinoRefusal;
using rtype::client::OwnBetsReading;
using rtype::client::testing::CasinoChannelFixture;
using rtype::client::testing::CHANNEL_DRAWN;
using rtype::client::testing::CHANNEL_FRAME;
using rtype::client::testing::CHANNEL_MASTER_TIME;
using rtype::client::testing::CHANNEL_ROOM;
using rtype::client::testing::CHANNEL_ROUND;
using rtype::client::testing::CHANNEL_TOKEN;
using rtype::client::testing::EARLIER_MASTER_TIME;
using rtype::client::testing::JUST_BEFORE_THE_READ;
using rtype::client::testing::JUST_BEFORE_THE_RENEWAL;
using rtype::client::testing::PAST_THE_CLOSE;
using rtype::client::testing::ROOM_READ_AGAIN_AFTER;
using rtype::client::testing::SEAT_RENEWED_AFTER;
using rtype::client::testing::SEATED_IN_CHANNEL_ROOM;
using rtype::client::testing::TEN_ON_BLACK;
using rtype::client::testing::TEN_ON_BLACK_PAID;
using rtype::client::testing::THREE_SECONDS;
using rtype::client::testing::UP_FROM_CHANNEL_ROOM;
using rtype::network::CasinoRoomState;
using rtype::network::RoomRound;
using rtype::network::RoulettePhase;
using rtype::network::RoundBets;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using MillisecondDate = std::chrono::sys_time<std::chrono::milliseconds>;

/**
 * Given a player signed in
 * When it enters a room
 * Then the room is read, and a seat is asked there with its token
 */
TEST_F(CasinoChannelFixture, EnteringReadsTheRoomAndTakesASeat) {
  channel().enter(CHANNEL_ROOM);

  EXPECT_THAT(link().roomsRead, ElementsAre(CHANNEL_ROOM));
  EXPECT_THAT(link().seatsAsked, ElementsAre(SEATED_IN_CHANNEL_ROOM));
  EXPECT_THAT(link().seatTokens, ElementsAre(std::string{CHANNEL_TOKEN}));
}

/**
 * Given a room entered whose read the master has not answered
 * When a second passes
 * Then the room is not read again: one read is in flight at most
 */
TEST_F(CasinoChannelFixture, ReadsTheRoomOnceAtATime) {
  channel().enter(CHANNEL_ROOM);

  wait(std::chrono::seconds(1));

  EXPECT_EQ(link().roomsRead.size(), 1U);
}

/**
 * Given the read of the room answered
 * When 480 ms pass, then 20 ms more
 * Then the room is read again only once 500 ms passed since the answer
 */
TEST_F(CasinoChannelFixture, ReadsTheRoomAgainHalfASecondAfterTheAnswer) {
  channel().enter(CHANNEL_ROOM);
  answerTheRead(roomAt(CHANNEL_MASTER_TIME));
  wait(CHANNEL_FRAME);

  wait(JUST_BEFORE_THE_READ);
  ASSERT_EQ(link().roomsRead.size(), 1U);
  wait(ROOM_READ_AGAIN_AFTER - JUST_BEFORE_THE_READ);

  EXPECT_EQ(link().roomsRead.size(), 2U);
}

/**
 * Given a seat asked as the room was entered
 * When 9.9 s pass, then 0.1 s more
 * Then the seat is asked again only once 10 s passed
 */
TEST_F(CasinoChannelFixture, RenewsTheSeatEveryTenSeconds) {
  channel().enter(CHANNEL_ROOM);

  wait(JUST_BEFORE_THE_RENEWAL);
  ASSERT_EQ(link().seatsAsked.size(), 1U);
  wait(SEAT_RENEWED_AFTER - JUST_BEFORE_THE_RENEWAL);

  EXPECT_THAT(link().seatsAsked,
              ElementsAre(SEATED_IN_CHANNEL_ROOM, SEATED_IN_CHANNEL_ROOM));
}

/**
 * Given the room as the master answered it
 * When 3 s pass on the clock of the client
 * Then the room shows the clock of the master 3 s later
 */
TEST_F(CasinoChannelFixture, TimesTheRoomOnTheClockOfTheMaster) {
  channel().enter(CHANNEL_ROOM);
  answerTheRead(roomAt(CHANNEL_MASTER_TIME));
  wait(CHANNEL_FRAME);

  wait(THREE_SECONDS);

  ASSERT_TRUE(channel().room().has_value());
  EXPECT_EQ(channel().room().value_or(CasinoRoomState{}).serverTime,
            MillisecondDate{CHANNEL_MASTER_TIME + THREE_SECONDS});
}

/**
 * Given the room as the master answered it, its bets open for 12 s
 * When 13 s pass with no other answer
 * Then the room shows the wheel turning
 */
TEST_F(CasinoChannelFixture, TheBetsCloseOnTheClockOfTheMaster) {
  channel().enter(CHANNEL_ROOM);
  answerTheRead(roomAt(CHANNEL_MASTER_TIME));
  wait(CHANNEL_FRAME);

  wait(PAST_THE_CLOSE);

  EXPECT_EQ(channel().room().value_or(CasinoRoomState{}).phase,
            RoulettePhase::Spinning);
}

/**
 * Given the room read, then read again
 * When the master answers the second read with an earlier time
 * Then the earlier answer is dropped: the room keeps the clock of the first
 */
TEST_F(CasinoChannelFixture, DropsAnAnswerOlderThanTheOneKept) {
  channel().enter(CHANNEL_ROOM);
  answerTheRead(roomAt(CHANNEL_MASTER_TIME));
  wait(CHANNEL_FRAME);
  wait(ROOM_READ_AGAIN_AFTER);

  answerTheRead(roomAt(EARLIER_MASTER_TIME));
  wait(std::chrono::milliseconds::zero());

  EXPECT_EQ(channel().room().value_or(CasinoRoomState{}).serverTime,
            MillisecondDate{CHANNEL_MASTER_TIME + ROOM_READ_AGAIN_AFTER});
}

/**
 * Given a room entered
 * When the player lays bets the master takes
 * Then the wallet is asked again, since the master staked them
 */
TEST_F(CasinoChannelFixture, AsksTheWalletAgainOnceBetsAreTaken) {
  channel().enter(CHANNEL_ROOM);
  channel().placeBets(CHANNEL_ROUND, {TEN_ON_BLACK});

  link().answers.bets = tenOnBlackTaken();
  wait(CHANNEL_FRAME);

  EXPECT_EQ(wallet().walletAsks, 1U);
}

/**
 * Given bets the master took on the round in progress
 * When the room shows the number drawn
 * Then the bets of the player on that round are read
 */
TEST_F(CasinoChannelFixture, ReadsTheOwnBetsOnceTheNumberIsDrawn) {
  channel().enter(CHANNEL_ROOM);
  channel().placeBets(CHANNEL_ROUND, {TEN_ON_BLACK});
  link().answers.bets = tenOnBlackTaken();
  wait(CHANNEL_FRAME);
  ASSERT_THAT(link().ownBetRounds, IsEmpty());

  CasinoRoomState drawn = roomAt(CHANNEL_MASTER_TIME);
  drawn.winningNumber = CHANNEL_DRAWN;
  answerTheRead(drawn);
  wait(CHANNEL_FRAME);

  EXPECT_THAT(
      link().ownBetRounds,
      ElementsAre(RoomRound{.room = CHANNEL_ROOM, .round = CHANNEL_ROUND}));
}

/**
 * Given the bets of a round read once its number was drawn
 * When the master answers they paid
 * Then the wallet is asked again, and the bets read show what they paid
 */
TEST_F(CasinoChannelFixture, APayoutAsksTheWalletAgain) {
  channel().enter(CHANNEL_ROOM);
  channel().placeBets(CHANNEL_ROUND, {TEN_ON_BLACK});
  link().answers.bets = tenOnBlackTaken();
  CasinoRoomState drawn = roomAt(CHANNEL_MASTER_TIME);
  drawn.winningNumber = CHANNEL_DRAWN;
  answerTheRead(drawn);
  wait(CHANNEL_FRAME);
  wait(CHANNEL_FRAME);

  link().answers.ownBets = OwnBetsReading{
      .ask = 1,
      .round = RoomRound{.room = CHANNEL_ROOM, .round = CHANNEL_ROUND},
      .bets = RoundBets{.round = CHANNEL_ROUND,
                        .bets = {TEN_ON_BLACK},
                        .stake = TEN_ON_BLACK.amount,
                        .payout = TEN_ON_BLACK_PAID,
                        .winningNumber = CHANNEL_DRAWN}};
  wait(CHANNEL_FRAME);

  EXPECT_EQ(wallet().walletAsks, 2U);
  EXPECT_EQ(channel().ownBets(CHANNEL_ROUND).value_or(RoundBets{}).payout,
            TEN_ON_BLACK_PAID);
}

/**
 * Given a guest
 * When it lays bets
 * Then nothing is asked of the master, and the answer says to sign in
 */
TEST_F(CasinoChannelFixture, AGuestIsToldToSignIn) {
  session().signOut();
  channel().enter(CHANNEL_ROOM);

  channel().placeBets(CHANNEL_ROUND, {TEN_ON_BLACK});

  EXPECT_THAT(link().betRounds, IsEmpty());
  EXPECT_EQ(channel().lastBets().value_or(BetsReading{}).refusal,
            CasinoRefusal::SignedOut);
}

/**
 * Given a player seated in a room
 * When it leaves
 * Then the master is asked to get it up from that room
 */
TEST_F(CasinoChannelFixture, LeavingGetsThePlayerUp) {
  channel().enter(CHANNEL_ROOM);

  channel().leave();

  EXPECT_THAT(link().seatsAsked,
              ElementsAre(SEATED_IN_CHANNEL_ROOM, UP_FROM_CHANNEL_ROOM));
}
