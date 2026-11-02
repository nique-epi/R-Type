#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "GameResult.hpp"
#include "LeaderboardAnswer.hpp"
#include "ProtocolConstants.hpp"
#include "ResultsDatabase.hpp"
#include "ResultsDatabaseFixture.hpp"
#include "ResultsDatabaseTestConstants.hpp"

namespace rtype::server::testing {

using network::LeaderboardAnswer;
using network::LeaderboardRow;
using network::MAXIMUM_LEADERBOARD_ROWS;

/**
 * Given a finished game
 * When it is saved
 * Then its row holds what the issue lists: a game identifier, an empty account,
 * the score, the deaths, the date, the difficulty and the count of players, not
 * vanilla, and not sent to the master
 */
TEST_F(ResultsDatabaseFixture, SavesARowWithEveryFieldOfTheResult) {
  ResultsDatabase database{file()};
  database.save(normalGame({PlayerResult{.nickname = std::string{ACE},
                                         .score = HIGH_SCORE,
                                         .deaths = TWO_DEATHS}},
                           SOME_DATE));

  const SQLite::Database reader{file().string()};
  SQLite::Statement row{reader, std::string{SELECT_ROW_OF_ACE}};
  ASSERT_TRUE(row.executeStep());
  EXPECT_EQ(row.getColumn("game_id").getString().size(), GAME_ID_SIZE);
  EXPECT_TRUE(row.getColumn("account_id").isNull());
  EXPECT_EQ(row.getColumn("score").getUInt(), HIGH_SCORE);
  EXPECT_EQ(row.getColumn("deaths").getUInt(), TWO_DEATHS);
  EXPECT_EQ(row.getColumn("played_at").getInt64(), SOME_DATE);
  EXPECT_EQ(row.getColumn("difficulty").getString(), NORMAL);
  EXPECT_EQ(row.getColumn("player_count").getInt(), TWO_PLAYERS);
  EXPECT_EQ(row.getColumn("vanilla").getInt(), 0);
  EXPECT_TRUE(row.getColumn("sent_at").isNull());
}

/**
 * Given two finished games, of two players each
 * When they are saved
 * Then there are two game identifiers: the players of one game share theirs
 */
TEST_F(ResultsDatabaseFixture, GivesEachGameOneIdentifierOfItsOwn) {
  ResultsDatabase database{file()};
  const std::vector players{playerNamed(ACE, LOW_SCORE),
                            playerNamed(BOB, LOW_SCORE)};
  database.save(normalGame(players, SOME_DATE));
  database.save(normalGame(players, LATER_DATE));

  SQLite::Database reader{file().string()};
  EXPECT_EQ(reader.execAndGet(std::string{COUNT_GAME_IDS}).getInt(), 2);
}

/**
 * Given a game at the normal difficulty and a game at the hard one
 * When the normal leaderboard is read
 * Then the hard score is not on it
 */
TEST_F(ResultsDatabaseFixture, NeverMixesTwoDifficulties) {
  ResultsDatabase database{file()};
  database.save(normalGame({playerNamed(ACE, LOW_SCORE)}, SOME_DATE));
  GameResult hard = normalGame({playerNamed(BOB, HIGH_SCORE)}, SOME_DATE);
  hard.difficulty = std::string{HARD};
  database.save(hard);

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{STRANGER});

  ASSERT_EQ(answer.rows.size(), 1U);
  EXPECT_EQ(answer.rows.at(0).nickname, ACE);
  EXPECT_EQ(answer.asked, normalBoard());
}

/**
 * Given a game of two players and a game of three
 * When the leaderboard of two players is read
 * Then the score of the game of three is not on it
 */
TEST_F(ResultsDatabaseFixture, NeverMixesTwoCountsOfPlayers) {
  ResultsDatabase database{file()};
  database.save(normalGame({playerNamed(ACE, LOW_SCORE)}, SOME_DATE));
  GameResult ofThree = normalGame({playerNamed(BOB, HIGH_SCORE)}, SOME_DATE);
  ofThree.playerCount = THREE_PLAYERS;
  database.save(ofThree);

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{STRANGER});

  ASSERT_EQ(answer.rows.size(), 1U);
  EXPECT_EQ(answer.rows.at(0).nickname, ACE);
}

/**
 * Given a player with a low score and a later high one
 * When the leaderboard is read
 * Then that player appears once, with the high score and its date
 */
TEST_F(ResultsDatabaseFixture, ListsEachPlayerOnceWithTheirBestScore) {
  ResultsDatabase database{file()};
  database.save(normalGame({playerNamed(ACE, LOW_SCORE)}, SOME_DATE));
  database.save(normalGame({playerNamed(ACE, HIGH_SCORE)}, LATER_DATE));

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{STRANGER});

  ASSERT_EQ(answer.rows.size(), 1U);
  EXPECT_EQ(answer.rows.at(0).score, HIGH_SCORE);
  EXPECT_EQ(answer.rows.at(0).playedAt, LATER_DATE);
}

/**
 * Given two players with one score, the later of them listed first in the file,
 * and a third player below them
 * When the leaderboard is read
 * Then the earlier game comes first, both rank 1, and the third ranks 3
 */
TEST_F(ResultsDatabaseFixture, PutsTheFirstArrivedFirstAndSkipsARankAfterATie) {
  ResultsDatabase database{file()};
  database.save(normalGame({playerNamed(BOB, HIGH_SCORE)}, LATER_DATE));
  database.save(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));
  database.save(normalGame({playerNamed(CID, LOW_SCORE)}, EVEN_LATER_DATE));

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{STRANGER});

  ASSERT_EQ(answer.rows.size(), 3U);
  EXPECT_EQ(answer.rows.at(0).nickname, ACE);
  EXPECT_EQ(answer.rows.at(0).rank, 1U);
  EXPECT_EQ(answer.rows.at(1).nickname, BOB);
  EXPECT_EQ(answer.rows.at(1).rank, 1U);
  EXPECT_EQ(answer.rows.at(2).rank, 3U);
}

/**
 * Given eleven players with different scores, the asker last
 * When the leaderboard is read
 * Then ten rows are listed, and the asker gets its own row, rank 11
 */
TEST_F(ResultsDatabaseFixture, ListsTenRowsAndTheRowOfAnAskerBelowThem) {
  ResultsDatabase database{file()};
  std::vector<PlayerResult> players;
  players.reserve(PLAYERS_ON_A_FULL_BOARD);
  for (std::size_t index = 0; index < PLAYERS_ON_A_FULL_BOARD; ++index) {
    players.push_back(
        playerNamed("P" + std::to_string(index),
                    HIGH_SCORE - static_cast<std::uint32_t>(index)));
  }
  database.save(normalGame(players, SOME_DATE));

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), players.back().nickname);

  EXPECT_EQ(answer.rows.size(), MAXIMUM_LEADERBOARD_ROWS);
  EXPECT_EQ(answer.ownRow.value_or(LeaderboardRow{}).rank,
            LAST_RANK_ON_A_FULL_BOARD);
}

/**
 * Given a leaderboard on which the asker holds the first place
 * When it is read
 * Then the asker is both listed and given its own row
 */
TEST_F(ResultsDatabaseFixture, GivesTheOwnRowOfAnAskerAmongTheBestRows) {
  ResultsDatabase database{file()};
  database.save(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{ACE});

  ASSERT_EQ(answer.rows.size(), 1U);
  EXPECT_EQ(answer.ownRow.value_or(LeaderboardRow{}), answer.rows.at(0));
}

/**
 * Given a leaderboard the asker is not on
 * When it is read
 * Then the asker has no row
 */
TEST_F(ResultsDatabaseFixture, GivesNoRowToAnAskerWhoNeverPlayed) {
  ResultsDatabase database{file()};
  database.save(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{STRANGER});

  EXPECT_FALSE(answer.ownRow.has_value());
}

/**
 * Given a score under ACE
 * When ace asks, as the roster takes for the same nickname
 * Then ace finds that row
 */
TEST_F(ResultsDatabaseFixture, FindsTheAskerWhateverTheCaseOfItsNickname) {
  ResultsDatabase database{file()};
  database.save(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{ACE_IN_LOWERCASE});

  EXPECT_TRUE(answer.ownRow.has_value());
}

/**
 * Given a leaderboard with no game
 * When it is read
 * Then it is empty and says which leaderboard it is
 */
TEST_F(ResultsDatabaseFixture, AnswersAnEmptyLeaderboard) {
  const ResultsDatabase database{file()};

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{ACE});

  EXPECT_TRUE(answer.rows.empty());
  EXPECT_EQ(answer.asked, normalBoard());
}

/**
 * Given a saved game and a database closed
 * When the file is opened again
 * Then the leaderboard still lists it, as after a restart of the server
 */
TEST_F(ResultsDatabaseFixture, KeepsTheLeaderboardAcrossARestart) {
  {
    ResultsDatabase database{file()};
    database.save(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));
  }

  const ResultsDatabase reopened{file()};
  const LeaderboardAnswer answer =
      reopened.leaderboard(normalBoard(), std::string{ACE});

  EXPECT_EQ(answer.rows.size(), 1U);
}

/**
 * Given a process that saves a game, then ends at once, closing nothing
 * When the file is opened again
 * Then the game is there and the file is usable. This covers a process killed:
 * an operating system crash or a power cut cannot be produced here
 */
TEST_F(ResultsDatabaseFixture, KeepsASavedGameWhenTheProcessDiesAtOnce) {
  EXPECT_EXIT(
      saveThenDieAtOnce(file(),
                        normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE)),
      ::testing::ExitedWithCode(DIED_AT_ONCE_EXIT_CODE), "");

  const ResultsDatabase reopened{file()};
  const LeaderboardAnswer answer =
      reopened.leaderboard(normalBoard(), std::string{ACE});

  EXPECT_EQ(answer.rows.size(), 1U);
}

/**
 * Given a game with a nickname written as SQL
 * When it is saved, then read
 * Then the nickname is plain text and the table is still there
 */
TEST_F(ResultsDatabaseFixture, TakesANicknameAsTextNeverAsSql) {
  ResultsDatabase database{file()};
  database.save(
      normalGame({playerNamed(SQL_IN_A_NICKNAME, LOW_SCORE)}, SOME_DATE));

  const LeaderboardAnswer answer =
      database.leaderboard(normalBoard(), std::string{SQL_IN_A_NICKNAME});

  ASSERT_EQ(answer.rows.size(), 1U);
  EXPECT_EQ(answer.rows.at(0).nickname, SQL_IN_A_NICKNAME);
}

/**
 * Given two games of one player, at two difficulties, under two cases of its
 * nickname
 * When its statistics are read
 * Then they add up both games under one player, difficulties together
 */
TEST_F(ResultsDatabaseFixture, AddsUpEveryGameOfAPlayerInItsStatistics) {
  ResultsDatabase database{file()};
  database.save(normalGame({PlayerResult{.nickname = std::string{ACE},
                                         .score = LOW_SCORE,
                                         .deaths = TWO_DEATHS}},
                           SOME_DATE));
  GameResult hard =
      normalGame({PlayerResult{.nickname = std::string{ACE_IN_LOWERCASE},
                               .score = MIDDLE_SCORE,
                               .deaths = ONE_DEATH}},
                 LATER_DATE);
  hard.difficulty = std::string{HARD};
  database.save(hard);

  const SQLite::Database reader{file().string()};
  SQLite::Statement statistics{reader, std::string{SELECT_STATISTICS_OF_ACE}};
  ASSERT_TRUE(statistics.executeStep());
  EXPECT_EQ(statistics.getColumn("games_played").getInt(), 2);
  EXPECT_EQ(statistics.getColumn("best_score").getUInt(), MIDDLE_SCORE);
  EXPECT_EQ(statistics.getColumn("total_score").getUInt(),
            LOW_SCORE + MIDDLE_SCORE);
  EXPECT_EQ(statistics.getColumn("deaths").getUInt(), TWO_DEATHS + ONE_DEATH);
  EXPECT_FALSE(statistics.executeStep());
}

/**
 * Given two nicknames that hold one account, and a guest under another nickname
 * When the statistics are read
 * Then the account is one player of two games, and the guest a player of its
 * own
 */
TEST_F(ResultsDatabaseFixture,
       FilesTheStatisticsOfAnAccountUnderItsIdentifier) {
  ResultsDatabase database{file()};
  database.save(
      normalGame({playerNamed(ACE, LOW_SCORE), playerNamed(BOB, LOW_SCORE),
                  playerNamed(CID, LOW_SCORE)},
                 SOME_DATE));
  SQLite::Database writer{file().string(), SQLite::OPEN_READWRITE};
  writer.exec(std::string{GIVE_ROWS_OF_BOB_AND_CID_AN_ACCOUNT});

  EXPECT_EQ(writer.execAndGet(std::string{COUNT_STATISTICS}).getInt(), 2);
  SQLite::Statement account{writer,
                            std::string{SELECT_STATISTICS_OF_THE_ACCOUNT}};
  ASSERT_TRUE(account.executeStep());
  EXPECT_EQ(account.getColumn("player").getString(), ACCOUNT_ID);
  EXPECT_EQ(account.getColumn("games_played").getInt(), 2);
}

}  // namespace rtype::server::testing
