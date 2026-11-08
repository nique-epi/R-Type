#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "EntityAnnouncements.hpp"
#include "PilotStatus.hpp"
#include "PlayerLeft.hpp"
#include "ScoreBoard.hpp"
#include "ScoreBoardFixtures.hpp"
#include "ScoreBoardTestConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFilterTestConstants.hpp"

using rtype::client::PilotState;
using rtype::client::PilotStatus;
using rtype::client::ScoreBoard;
using rtype::client::testing::APPLIED_TICK;
using rtype::client::testing::NEWER_TICK;
using rtype::client::testing::OLDER_TICK;
using rtype::client::testing::stateListingPlayers;
using rtype::network::DepartureReason;
using rtype::network::PlayerLeft;
using rtype::network::PlayerState;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;

/**
 * Given a waiting room listing players 3, 1 and 2
 * When the board is built from it
 * Then it holds the three pilots in the order of their numbers
 */
TEST(ScoreBoard, SeatsThePlayersOfTheWaitingRoomInNumberOrder) {
  const ScoreBoard board = scoreBoardOfThree();

  EXPECT_THAT(board.pilots(),
              ElementsAre(Field(&PilotStatus::playerNumber, PLAYER_ONE),
                          Field(&PilotStatus::playerNumber, PLAYER_TWO),
                          Field(&PilotStatus::playerNumber, PLAYER_THREE)));
}

/**
 * Given a waiting room listing KAI as player 3, ACE as 1 and NOVA as 2
 * When the board is built from it
 * Then each pilot keeps its nickname
 */
TEST(ScoreBoard, KeepsTheNicknamesOfTheWaitingRoom) {
  const ScoreBoard board = scoreBoardOfThree();

  EXPECT_THAT(board.pilots(),
              ElementsAre(Field(&PilotStatus::nickname, "ACE"),
                          Field(&PilotStatus::nickname, "NOVA"),
                          Field(&PilotStatus::nickname, "KAI")));
}

/**
 * Given a board just built, before any world state
 * When its first pilot is read
 * Then its score is 0, its lives unknown, it flies, and it is not this client
 */
TEST(ScoreBoard, APilotStartsWithNothingKnown) {
  const ScoreBoard board = scoreBoardOfThree();

  EXPECT_EQ(board.pilots().front(), (PilotStatus{.playerNumber = PLAYER_ONE,
                                                 .nickname = "ACE",
                                                 .score = 0,
                                                 .lives = std::nullopt,
                                                 .state = PilotState::Flying,
                                                 .isLocalPlayer = false}));
}

/**
 * Given a board where this client is player 2
 * When the pilots are read
 * Then player 2, and it alone, is the local player
 */
TEST(ScoreBoard, TagsTheLocalPlayer) {
  const ScoreBoard board = scoreBoardOfThree();

  EXPECT_THAT(board.pilots(),
              ElementsAre(Field(&PilotStatus::isLocalPlayer, false),
                          Field(&PilotStatus::isLocalPlayer, true),
                          Field(&PilotStatus::isLocalPlayer, false)));
}

/**
 * Given a board of three pilots
 * When a world state lists player 1 with 3 lives and 12400 points
 * Then player 1 has that score and those lives
 */
TEST(ScoreBoard, AWorldStateSetsTheScoreAndTheLives) {
  ScoreBoard board = scoreBoardOfThree();

  board.apply(
      stateListingPlayers(APPLIED_TICK, {listedPlayer(PLAYER_ONE, LIVES)}));

  EXPECT_EQ(board.pilots().front().score, SCORE);
  EXPECT_EQ(board.pilots().front().lives, LIVES);
}

/**
 * Given a board that applied a state where player 1 has 1 life
 * When an older state, where it had 3, arrives late
 * Then player 1 still has 1 life
 */
TEST(ScoreBoard, AnOlderWorldStateIsIgnored) {
  ScoreBoard board = scoreBoardOfThree();
  board.apply(
      stateListingPlayers(APPLIED_TICK, {listedPlayer(PLAYER_ONE, LAST_LIFE)}));

  board.apply(
      stateListingPlayers(OLDER_TICK, {listedPlayer(PLAYER_ONE, LIVES)}));

  EXPECT_EQ(board.pilots().front().lives, LAST_LIFE);
}

/**
 * Given a board of three pilots
 * When a world state lists player 1 invulnerable
 * Then player 1 is respawning
 */
TEST(ScoreBoard, AnInvulnerablePilotIsRespawning) {
  ScoreBoard board = scoreBoardOfThree();
  PlayerState respawned = listedPlayer(PLAYER_ONE, LIVES);
  respawned.invulnerable = true;

  board.apply(stateListingPlayers(APPLIED_TICK, {respawned}));

  EXPECT_EQ(board.pilots().front().state, PilotState::Respawning);
}

/**
 * Given a board of three pilots
 * When a world state lists player 1 with no life left
 * Then player 1 is out of lives
 */
TEST(ScoreBoard, NoLifeLeftIsOutOfLives) {
  ScoreBoard board = scoreBoardOfThree();

  board.apply(
      stateListingPlayers(APPLIED_TICK, {listedPlayer(PLAYER_ONE, NO_LIFE)}));

  EXPECT_EQ(board.pilots().front().state, PilotState::OutOfLives);
}

/**
 * Given a board of players 1 to 3
 * When a world state lists player 4, whom the waiting room did not list
 * Then player 4 is seated last, with its score
 */
TEST(ScoreBoard, APlayerMissingFromTheWaitingRoomIsSeated) {
  ScoreBoard board = scoreBoardOfThree();

  board.apply(
      stateListingPlayers(APPLIED_TICK, {listedPlayer(PLAYER_FOUR, LIVES)}));

  ASSERT_EQ(board.pilots().size(), PILOTS_OF_FOUR);
  EXPECT_EQ(board.pilots().back().playerNumber, PLAYER_FOUR);
  EXPECT_EQ(board.pilots().back().score, SCORE);
  EXPECT_TRUE(board.pilots().back().nickname.empty());
}

/**
 * Given a board of three pilots
 * When player 3 leaves, then player 1 times out
 * Then player 3 has left and player 1 has lost its signal
 */
TEST(ScoreBoard, ADepartureSaysHowThePilotLeft) {
  ScoreBoard board = scoreBoardOfThree();

  board.apply(PlayerLeft{.playerNumber = PLAYER_THREE,
                         .reason = DepartureReason::Left});
  board.apply(PlayerLeft{.playerNumber = PLAYER_ONE,
                         .reason = DepartureReason::TimedOut});

  EXPECT_EQ(board.pilots().back().state, PilotState::Left);
  EXPECT_EQ(board.pilots().front().state, PilotState::SignalLost);
}

/**
 * Given a board of three pilots
 * When player 3 leaves because the host expelled it
 * Then player 3 is expelled
 */
TEST(ScoreBoard, AnExpelledPilotIsExpelled) {
  ScoreBoard board = scoreBoardOfThree();

  board.apply(PlayerLeft{.playerNumber = PLAYER_THREE,
                         .reason = DepartureReason::Expelled});

  EXPECT_EQ(board.pilots().back().state, PilotState::Expelled);
}

/**
 * Given a board where player 1 has left
 * When a newer world state still lists player 1
 * Then player 1 is still gone, with the score of that state
 */
TEST(ScoreBoard, ALaterStateDoesNotBringBackAPilotWhoLeft) {
  ScoreBoard board = scoreBoardOfThree();
  board.apply(
      PlayerLeft{.playerNumber = PLAYER_ONE, .reason = DepartureReason::Left});

  board.apply(
      stateListingPlayers(APPLIED_TICK, {listedPlayer(PLAYER_ONE, LIVES)}));

  EXPECT_EQ(board.pilots().front().state, PilotState::Left);
  EXPECT_EQ(board.pilots().front().score, SCORE);
}

/**
 * Given an empty board
 * When states and departures name player 0 and player 5, one step outside
 * each bound
 * Then no pilot is seated
 */
TEST(ScoreBoard, ANumberOutsideOneToFourIsIgnored) {
  ScoreBoard board({}, LOCAL_NUMBER);

  board.apply(stateListingPlayers(APPLIED_TICK,
                                  {listedPlayer(NO_PLAYER, LIVES),
                                   listedPlayer(PAST_THE_LAST_PLAYER, LIVES)}));
  board.apply(PlayerLeft{.playerNumber = PAST_THE_LAST_PLAYER,
                         .reason = DepartureReason::Left});

  EXPECT_THAT(board.pilots(), IsEmpty());
}

/**
 * Given a board of three pilots
 * When a world state gives player 1 its first score and lives
 * Then the revision moves
 */
TEST(ScoreBoard, AChangeMovesTheRevision) {
  ScoreBoard board = scoreBoardOfThree();
  const std::uint32_t before = board.revision();

  board.apply(
      stateListingPlayers(APPLIED_TICK, {listedPlayer(PLAYER_ONE, LIVES)}));

  EXPECT_NE(board.revision(), before);
}

/**
 * Given a board that applied a state listing player 1
 * When a newer state lists the same score and lives
 * Then the revision stays, so nothing is repainted
 */
TEST(ScoreBoard, AStateThatChangesNothingKeepsTheRevision) {
  ScoreBoard board = scoreBoardOfThree();
  board.apply(
      stateListingPlayers(APPLIED_TICK, {listedPlayer(PLAYER_ONE, LIVES)}));
  const std::uint32_t before = board.revision();

  board.apply(
      stateListingPlayers(NEWER_TICK, {listedPlayer(PLAYER_ONE, LIVES)}));

  EXPECT_EQ(board.revision(), before);
}
