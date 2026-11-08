#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "GameCatalog.hpp"
#include "GameCatalogFixture.hpp"
#include "GameCatalogTestConstants.hpp"
#include "GameListing.hpp"
#include "GameRules.hpp"
#include "GameRulesTestConstants.hpp"
#include "ListedGameState.hpp"
#include "ServerListPhase.hpp"

using rtype::client::isJoinable;
using rtype::client::ServerListPhase;
using rtype::client::testing::FIRST_GAME_ASK;
using rtype::client::testing::FIRST_GAME_NUMBER;
using rtype::client::testing::FIRST_SERVER_ID;
using rtype::client::testing::GameCatalogFixture;
using rtype::client::testing::PLAYERS_PER_GAME_OF_THE_SERVER;
using rtype::client::testing::SECOND_GAME_NUMBER;
using rtype::client::testing::SECOND_SERVER_ID;
using rtype::network::GameListing;
using rtype::network::GameRules;
using rtype::network::ListedGameState;
using rtype::testing::CHOSEN_DIFFICULTY;
using rtype::testing::CHOSEN_LIVES;
using rtype::testing::EASY_DIFFICULTY;
using rtype::testing::NORMAL_DIFFICULTY;
using rtype::testing::VETERAN_DIFFICULTY;
using ::testing::ElementsAre;

/**
 * Given a catalog that asked for the games of a server
 * When the master has not answered yet
 * Then the games are searching
 */
TEST_F(GameCatalogFixture, IsSearchingUntilTheMasterAnswers) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  catalog().update();

  EXPECT_EQ(catalog().snapshot().phase, ServerListPhase::Searching);
  EXPECT_EQ(master().gameAsks().front(), FIRST_SERVER_ID);
}

/**
 * Given the master answered two waiting games
 * When the catalog takes them
 * Then each row takes the maximum of players of its server
 */
TEST_F(GameCatalogFixture, GivesEachGameTheMaximumOfItsServer) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  master().answerGames(FIRST_GAME_ASK, twoGames(ListedGameState::Waiting,
                                                ListedGameState::Waiting));
  catalog().update();

  ASSERT_EQ(catalog().snapshot().phase, ServerListPhase::Ready);
  ASSERT_EQ(catalog().snapshot().games.size(), 2U);
  EXPECT_EQ(catalog().snapshot().games.front().maximumPlayers,
            PLAYERS_PER_GAME_OF_THE_SERVER);
}

/**
 * Given game 1 in game and game 2 waiting
 * When the catalog takes them
 * Then game 2, which can be joined, comes first
 */
TEST_F(GameCatalogFixture, PutsTheJoinableGamesFirst) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  master().answerGames(FIRST_GAME_ASK, twoGames(ListedGameState::InGame,
                                                ListedGameState::Waiting));
  catalog().update();

  ASSERT_EQ(catalog().snapshot().games.size(), 2U);
  EXPECT_EQ(catalog().snapshot().games.front().number, SECOND_GAME_NUMBER);
}

/**
 * Given games 1 and 2 both waiting
 * When the catalog takes them
 * Then they keep the order of their numbers
 */
TEST_F(GameCatalogFixture, KeepsTheOrderOfNumbersAmongJoinableGames) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  master().answerGames(FIRST_GAME_ASK, twoGames(ListedGameState::Waiting,
                                                ListedGameState::Waiting));
  catalog().update();

  ASSERT_EQ(catalog().snapshot().games.size(), 2U);
  EXPECT_EQ(catalog().snapshot().games.front().number, FIRST_GAME_NUMBER);
}

/**
 * Given the games of a first server asked, then those of a second one
 * When the answer to the first ask comes
 * Then the catalog still searches the games of the second server
 */
TEST_F(GameCatalogFixture, IgnoresTheGamesOfAnOlderAsk) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  catalog().ask(serverOf(SECOND_SERVER_ID));
  master().answerGames(FIRST_GAME_ASK, twoGames(ListedGameState::Waiting,
                                                ListedGameState::Waiting));
  catalog().update();

  EXPECT_EQ(catalog().snapshot().phase, ServerListPhase::Searching);
}

/**
 * Given a catalog that asked for the games of a server
 * When the master does not answer the ask
 * Then the games say the master is unreachable
 */
TEST_F(GameCatalogFixture, SaysTheMasterIsUnreachableWhenItDidNotAnswer) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  master().answerGames(FIRST_GAME_ASK, std::nullopt);
  catalog().update();

  EXPECT_EQ(catalog().snapshot().phase, ServerListPhase::MasterUnreachable);
}

/**
 * Given a server with no game
 * When the catalog takes its games
 * Then the games are empty
 */
TEST_F(GameCatalogFixture, IsEmptyForAServerWithNoGame) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  master().answerGames(FIRST_GAME_ASK, GameListing{});
  catalog().update();

  EXPECT_EQ(catalog().snapshot().phase, ServerListPhase::Empty);
}

/**
 * Given a server that lists the difficulties easy, normal and veteran
 * When the catalog takes its games
 * Then it keeps those difficulties, in that order
 */
TEST_F(GameCatalogFixture, KeepsTheDifficultiesOfTheServer) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  master().answerGames(FIRST_GAME_ASK, aHardGame());
  catalog().update();

  EXPECT_THAT(
      catalog().snapshot().difficulties,
      ElementsAre(EASY_DIFFICULTY, NORMAL_DIFFICULTY, VETERAN_DIFFICULTY));
}

/**
 * Given a server with no game that lists its difficulties
 * When the catalog takes its games
 * Then it keeps those difficulties, so a new game can be opened there
 */
TEST_F(GameCatalogFixture, KeepsTheDifficultiesOfAServerWithNoGame) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  GameListing noGame = aHardGame();
  noGame.games.clear();
  master().answerGames(FIRST_GAME_ASK, noGame);
  catalog().update();

  ASSERT_EQ(catalog().snapshot().phase, ServerListPhase::Empty);
  EXPECT_EQ(catalog().snapshot().difficulties.size(), 3U);
}

/**
 * Given a game played hard, with five lives and friendly fire on
 * When the catalog takes it
 * Then its row is played by those rules
 */
TEST_F(GameCatalogFixture, GivesEachGameItsRules) {
  catalog().ask(serverOf(FIRST_SERVER_ID));
  master().answerGames(FIRST_GAME_ASK, aHardGame());
  catalog().update();

  ASSERT_EQ(catalog().snapshot().games.size(), 1U);
  EXPECT_EQ(catalog().snapshot().games.front().rules,
            (GameRules{.difficulty = std::string{CHOSEN_DIFFICULTY},
                       .lives = CHOSEN_LIVES,
                       .friendlyFire = true}));
}

/**
 * Given a game waiting for its players
 * When it is judged
 * Then it can be joined
 */
TEST_F(GameCatalogFixture, AWaitingGameCanBeJoined) {
  EXPECT_TRUE(isJoinable(gameIn(ListedGameState::Waiting)));
}

/**
 * Given a full game
 * When it is judged
 * Then it cannot be joined: it has no seat
 */
TEST_F(GameCatalogFixture, AFullGameCannotBeJoined) {
  EXPECT_FALSE(isJoinable(gameIn(ListedGameState::Full)));
}

/**
 * Given a game in progress
 * When it is judged
 * Then it cannot be joined: the server takes nobody into it
 */
TEST_F(GameCatalogFixture, AGameInProgressCannotBeJoined) {
  EXPECT_FALSE(isJoinable(gameIn(ListedGameState::InGame)));
}
