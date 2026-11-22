#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <gtest/gtest.h>
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "DatabaseFile.hpp"
#include "GameResult.hpp"
#include "ResultsDatabase.hpp"
#include "ResultsDatabaseConstants.hpp"
#include "ResultsDatabaseFixture.hpp"
#include "ResultsDatabaseTestConstants.hpp"
#include "ServerMigrationsConstants.hpp"
#include "StorageConstants.hpp"
#include "UnsentGame.hpp"

namespace rtype::server::testing {

/**
 * Given a vanilla game of two players, saved
 * When the games the master has not answered are read
 * Then it is the only one, under the id of its rows, with every field of its
 * result
 */
TEST_F(ResultsDatabaseFixture, ReadsAVanillaGameTheMasterHasNotAnswered) {
  ResultsDatabase database{file()};
  database.save(vanillaGame({PlayerResult{.nickname = std::string{ACE},
                                          .score = HIGH_SCORE,
                                          .deaths = TWO_DEATHS,
                                          .wonLooks = {}},
                             PlayerResult{.nickname = std::string{BOB},
                                          .score = LOW_SCORE,
                                          .deaths = ONE_DEATH,
                                          .wonLooks = {}}},
                            SOME_DATE));

  const std::vector<UnsentGame> games =
      database.unsentVanillaGames(EVERY_WAITING_GAME);

  ASSERT_EQ(games.size(), 1U);
  EXPECT_EQ(games[0].gameId.size(), GAME_ID_SIZE);
  EXPECT_EQ(games[0].result.difficulty, NORMAL);
  EXPECT_EQ(games[0].result.playerCount, TWO_PLAYERS);
  EXPECT_EQ(games[0].result.endedAt, SOME_DATE);
  EXPECT_TRUE(games[0].result.vanilla);
  EXPECT_EQ(
      games[0].result.players,
      (std::vector<PlayerResult>{PlayerResult{.nickname = std::string{ACE},
                                              .score = HIGH_SCORE,
                                              .deaths = TWO_DEATHS,
                                              .wonLooks = {}},
                                 PlayerResult{.nickname = std::string{BOB},
                                              .score = LOW_SCORE,
                                              .deaths = ONE_DEATH,
                                              .wonLooks = {}}}));
}

/**
 * Given a won vanilla game of a player who joined with a game ticket, earned
 * a bounty and won two look items, saved
 * When the games the master has not answered are read
 * Then the game says it was won, and its row keeps the ticket, the bounty and
 * both items in their order
 */
TEST_F(ResultsDatabaseFixture, ReadsTheTicketTheLooksAndTheVictoryOfAGame) {
  ResultsDatabase database{file()};
  GameResult won = vanillaGame(
      {PlayerResult{.nickname = std::string{ACE},
                    .score = HIGH_SCORE,
                    .bounty = SAVED_BOUNTY,
                    .deaths = TWO_DEATHS,
                    .wonLooks = {std::string{WON_HALO}, std::string{WON_CROWN}},
                    .ticket = std::string{SAVED_TICKET}}},
      SOME_DATE);
  won.victory = true;
  database.save(won);

  const std::vector<UnsentGame> games =
      database.unsentVanillaGames(EVERY_WAITING_GAME);

  ASSERT_EQ(games.size(), 1U);
  EXPECT_TRUE(games[0].result.victory);
  EXPECT_EQ(games[0].result.players, won.players);
}

/**
 * Given a lost vanilla game of a guest who won nothing, saved
 * When the games the master has not answered are read
 * Then the game says it was lost, and its row names no ticket and no item
 */
TEST_F(ResultsDatabaseFixture, ReadsALostGameOfAGuest) {
  ResultsDatabase database{file()};
  database.save(vanillaGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));

  const std::vector<UnsentGame> games =
      database.unsentVanillaGames(EVERY_WAITING_GAME);

  ASSERT_EQ(games.size(), 1U);
  EXPECT_FALSE(games[0].result.victory);
  EXPECT_EQ(games[0].result.players.at(0).ticket, std::nullopt);
  EXPECT_TRUE(games[0].result.players.at(0).wonLooks.empty());
}

/**
 * Given a game that is not vanilla, saved
 * When the games the master has not answered are read
 * Then there is none: the master keeps vanilla games only
 */
TEST_F(ResultsDatabaseFixture, NeverReadsAGameThatIsNotVanilla) {
  ResultsDatabase database{file()};
  database.save(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));

  EXPECT_TRUE(database.unsentVanillaGames(EVERY_WAITING_GAME).empty());
}

/**
 * Given three vanilla games, saved in another order than they ended
 * When two of the games the master has not answered are read
 * Then they are the two that ended first, the oldest first
 */
TEST_F(ResultsDatabaseFixture, ReadsTheOldestGamesFirstUpToTheLimit) {
  ResultsDatabase database{file()};
  database.save(vanillaGame({playerNamed(BOB, LOW_SCORE)}, LATER_DATE));
  database.save(vanillaGame({playerNamed(CID, LOW_SCORE)}, EVEN_LATER_DATE));
  database.save(vanillaGame({playerNamed(ACE, LOW_SCORE)}, SOME_DATE));

  const std::vector<UnsentGame> games =
      database.unsentVanillaGames(TWO_WAITING_GAMES);

  ASSERT_EQ(games.size(), TWO_WAITING_GAMES);
  EXPECT_EQ(games[0].result.endedAt, SOME_DATE);
  EXPECT_EQ(games[1].result.endedAt, LATER_DATE);
}

/**
 * Given two vanilla games, the first one sent to the master
 * When the games the master has not answered are read
 * Then only the second one is, and the rows of the first hold when it was sent
 */
TEST_F(ResultsDatabaseFixture, NeverReadsAGameTheMasterAccepted) {
  ResultsDatabase database{file()};
  database.save(vanillaGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));
  database.save(vanillaGame({playerNamed(BOB, LOW_SCORE)}, LATER_DATE));
  database.markSent(database.unsentVanillaGames(EVERY_WAITING_GAME)[0].gameId,
                    ANSWER_DATE);

  const std::vector<UnsentGame> games =
      database.unsentVanillaGames(EVERY_WAITING_GAME);

  ASSERT_EQ(games.size(), 1U);
  EXPECT_EQ(games[0].result.endedAt, LATER_DATE);
  const SQLite::Database reader{file().string(), SQLite::OPEN_READONLY,
                                storage::DATABASE_BUSY_TIMEOUT_MILLISECONDS};
  SQLite::Statement answers{reader, std::string{SELECT_ANSWERS_OF_ACE}};
  ASSERT_TRUE(answers.executeStep());
  EXPECT_EQ(answers.getColumn("sent_at").getInt64(),
            ANSWER_DATE.time_since_epoch().count());
  EXPECT_TRUE(answers.getColumn("refused_at").isNull());
}

/**
 * Given two vanilla games, the first one refused by the master for good
 * When the games the master has not answered are read
 * Then only the second one is, and the rows of the first hold when it was
 * refused, and no sending
 */
TEST_F(ResultsDatabaseFixture, NeverReadsAGameTheMasterRefused) {
  ResultsDatabase database{file()};
  database.save(vanillaGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));
  database.save(vanillaGame({playerNamed(BOB, LOW_SCORE)}, LATER_DATE));
  database.markRefused(
      database.unsentVanillaGames(EVERY_WAITING_GAME)[0].gameId, ANSWER_DATE);

  const std::vector<UnsentGame> games =
      database.unsentVanillaGames(EVERY_WAITING_GAME);

  ASSERT_EQ(games.size(), 1U);
  EXPECT_EQ(games[0].result.endedAt, LATER_DATE);
  const SQLite::Database reader{file().string(), SQLite::OPEN_READONLY,
                                storage::DATABASE_BUSY_TIMEOUT_MILLISECONDS};
  SQLite::Statement answers{reader, std::string{SELECT_ANSWERS_OF_ACE}};
  ASSERT_TRUE(answers.executeStep());
  EXPECT_EQ(answers.getColumn("refused_at").getInt64(),
            ANSWER_DATE.time_since_epoch().count());
  EXPECT_TRUE(answers.getColumn("sent_at").isNull());
}

/**
 * Given a file of the first version holding a vanilla game, as a server built
 * before the link with the master left it
 * When it is opened and the games the master has not answered are read
 * Then that game is among them
 */
TEST_F(ResultsDatabaseFixture, ReadsTheGamesOfAFileOfTheFirstVersion) {
  {
    SQLite::Database firstVersion = storage::openDatabaseFile(
        file().string(), std::array{RESULTS_TABLE_MIGRATION});
    firstVersion.exec(std::string{INSERT_VANILLA_ROW_OF_ACE});
  }
  const ResultsDatabase database{file()};

  const std::vector<UnsentGame> games =
      database.unsentVanillaGames(EVERY_WAITING_GAME);

  ASSERT_EQ(games.size(), 1U);
  EXPECT_EQ(games[0].gameId, ID_OF_THE_FIRST_VERSION_GAME);
}

/**
 * Given a file of the fourth version holding a vanilla game, as a server built
 * before the accounts left it
 * When it is opened and the games the master has not answered are read
 * Then that game reads as lost, by a guest who won no item and no bounty
 */
TEST_F(ResultsDatabaseFixture, ReadsAGameOfTheFourthVersionAsLostByAGuest) {
  {
    SQLite::Database fourthVersion = storage::openDatabaseFile(
        file().string(),
        std::array{RESULTS_TABLE_MIGRATION, UNSENT_GAMES_MIGRATION,
                   BANS_TABLE_MIGRATION, DROP_BANS_TABLE_MIGRATION});
    fourthVersion.exec(std::string{INSERT_VANILLA_ROW_OF_ACE});
  }
  const ResultsDatabase database{file()};

  const std::vector<UnsentGame> games =
      database.unsentVanillaGames(EVERY_WAITING_GAME);

  ASSERT_EQ(games.size(), 1U);
  EXPECT_FALSE(games[0].result.victory);
  EXPECT_EQ(games[0].result.players.at(0).ticket, std::nullopt);
  EXPECT_TRUE(games[0].result.players.at(0).wonLooks.empty());
  EXPECT_EQ(games[0].result.players.at(0).bounty, 0U);
}

}  // namespace rtype::server::testing
