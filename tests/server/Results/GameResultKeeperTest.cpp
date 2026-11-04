#include <gtest/gtest.h>
#include <chrono>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameResult.hpp"
#include "GameResultKeeper.hpp"
#include "GameResultKeeperFixture.hpp"
#include "GameResultKeeperTestConstants.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "PlayerRoster.hpp"
#include "RecordingResultsArchive.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

/**
 * Given a game of one player with a score, who lost every life
 * When it finishes
 * Then one result is recorded, with its nickname, score and deaths
 */
TEST_F(GameResultKeeperFixture, RecordsThePlayersWhenTheGameFinishes) {
  ConnectedPlayer& player = seat(FIRST_PLAYER);
  startTheGame();
  player.score = KEEPER_SCORE;
  player.livesWithoutShip = NO_LIFE;

  finishTheGame();

  const GameResult result = onlyResult();
  ASSERT_EQ(result.players.size(), 1U);
  EXPECT_EQ(result.players.at(0),
            (PlayerResult{.nickname = std::string{NICKNAMES.at(FIRST_PLAYER)},
                          .score = KEEPER_SCORE,
                          .deaths = EVERY_LIFE_LOST}));
}

/**
 * Given a player who never lost a life
 * When the game finishes
 * Then it counts no death
 */
TEST_F(GameResultKeeperFixture, CountsNoDeathForAPlayerWhoLostNoLife) {
  seat(FIRST_PLAYER);
  startTheGame();

  finishTheGame();

  EXPECT_EQ(onlyResult().players.at(0).deaths, 0U);
}

/**
 * Given a keeper of a vanilla game
 * When the game finishes
 * Then its result says the game was vanilla
 */
TEST_F(GameResultKeeperFixture, SaysTheResultOfAVanillaGameIsVanilla) {
  seat(FIRST_PLAYER);
  startTheGame();

  finishTheGame();

  EXPECT_TRUE(onlyResult().vanilla);
}

/**
 * Given a keeper of a game that is not vanilla
 * When the game finishes
 * Then its result says the game was not vanilla
 */
TEST(GameResultKeeper, SaysTheResultOfAGameThatIsNotVanillaIsNot) {
  PlayerRoster roster;
  RecordingResultsArchive archive;
  GameResultKeeper keeper{roster, archive,
                          GameSettings{.waves = {}, .vanilla = false}};
  ASSERT_NE(roster.add(playerEndpoint(FIRST_PLAYER),
                       std::string{NICKNAMES.at(FIRST_PLAYER)}, SESSION_TOKEN),
            nullptr);
  keeper.followState(GameState::Starting);
  keeper.followState(GameState::InProgress);

  keeper.followState(GameState::Finished);

  ASSERT_EQ(archive.results().size(), 1U);
  EXPECT_FALSE(archive.results().front().vanilla);
}

/**
 * Given three players at the start, one of whom leaves with a score
 * When the game finishes
 * Then the count of players stays 3, the three have a row, and the one who left
 * keeps the score it had when it left
 */
TEST_F(GameResultKeeperFixture, KeepsARowForAPlayerWhoLeftDuringTheGame) {
  ConnectedPlayer& leaving = seat(FIRST_PLAYER);
  seat(SECOND_PLAYER);
  seat(THIRD_PLAYER);
  startTheGame();
  leaving.score = KEEPER_SCORE;
  leave(FIRST_PLAYER);

  finishTheGame();

  const GameResult result = onlyResult();
  EXPECT_EQ(result.playerCount, THREE_AT_THE_START);
  ASSERT_EQ(result.players.size(), 3U);
  EXPECT_EQ(result.players.at(0).nickname, NICKNAMES.at(FIRST_PLAYER));
  EXPECT_EQ(result.players.at(0).score, KEEPER_SCORE);
}

/**
 * Given every player of a game gone before its end
 * When the game finishes
 * Then their rows are recorded all the same
 */
TEST_F(GameResultKeeperFixture, RecordsAGameEveryPlayerLeft) {
  seat(FIRST_PLAYER);
  startTheGame();
  leave(FIRST_PLAYER);

  finishTheGame();

  EXPECT_EQ(onlyResult().players.size(), 1U);
}

/**
 * Given a player who leaves before the game is in progress
 * When the game starts, then finishes without any other player
 * Then nothing is recorded: nobody played it
 */
TEST_F(GameResultKeeperFixture, RecordsNothingForAGameWithoutPlayers) {
  seat(FIRST_PLAYER);
  leave(FIRST_PLAYER);
  startTheGame();

  finishTheGame();

  EXPECT_TRUE(archive().results().empty());
}

/**
 * Given a player who leaves while the game shows its result
 * When the game waits for players again
 * Then the recorded result is unchanged, and the next one will not list it
 */
TEST_F(GameResultKeeperFixture, IgnoresADepartureAfterTheEnd) {
  seat(FIRST_PLAYER);
  seat(SECOND_PLAYER);
  startTheGame();
  finishTheGame();

  leave(FIRST_PLAYER);
  waitForPlayersAgain();
  startTheGame();
  finishTheGame();

  ASSERT_EQ(archive().results().size(), 2U);
  EXPECT_EQ(archive().results().at(1).players.size(), 1U);
}

/**
 * Given a finished game
 * When more ticks run in that state
 * Then it is recorded once
 */
TEST_F(GameResultKeeperFixture, RecordsAGameOnce) {
  seat(FIRST_PLAYER);
  startTheGame();

  finishTheGame();
  finishTheGame();
  finishTheGame();

  EXPECT_EQ(archive().results().size(), 1U);
}

/**
 * Given a player left during a first game, and a second game
 * When the second one finishes
 * Then it does not list the player of the first one
 */
TEST_F(GameResultKeeperFixture,
       StartsEachGameWithoutTheDeparturesOfTheLastOne) {
  seat(FIRST_PLAYER);
  seat(SECOND_PLAYER);
  startTheGame();
  leave(FIRST_PLAYER);
  finishTheGame();
  waitForPlayersAgain();

  startTheGame();
  finishTheGame();

  ASSERT_EQ(archive().results().size(), 2U);
  EXPECT_EQ(archive().results().at(1).players.size(), 1U);
  EXPECT_EQ(archive().results().at(1).playerCount, 1U);
}

/**
 * Given a game in progress the server stops
 * When nothing more reaches the keeper
 * Then nothing is recorded: the game has no end
 */
TEST_F(GameResultKeeperFixture, RecordsNothingForAGameThatDidNotEnd) {
  seat(FIRST_PLAYER);
  startTheGame();

  EXPECT_TRUE(archive().results().empty());
}

/**
 * Given a game
 * When it finishes
 * Then its result names the difficulty, and the date lies between the moments
 * around the end
 */
TEST_F(GameResultKeeperFixture, DatesAndNamesTheResultAtTheEnd) {
  seat(FIRST_PLAYER);
  startTheGame();
  const std::chrono::sys_seconds before =
      std::chrono::floor<std::chrono::seconds>(
          std::chrono::system_clock::now());

  finishTheGame();

  const GameResult result = onlyResult();
  EXPECT_EQ(result.difficulty, KEEPER_DIFFICULTY);
  EXPECT_GE(result.endedAt, before);
  EXPECT_LE(result.endedAt, std::chrono::system_clock::now());
}

}  // namespace rtype::server::testing
