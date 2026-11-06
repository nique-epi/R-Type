#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <string>
#include "EngineException.hpp"
#include "ExecutablePath.hpp"
#include "LogLevel.hpp"
#include "RecordedLine.hpp"
#include "ServerGameStatisticsFixture.hpp"
#include "ServerGameStatisticsModuleTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "TrafficTestConstants.hpp"

using rtype::engine::ModuleStartFailedException;

/**
 * Given the compiled plugin named in the configuration
 * When the server starts
 * Then its file holds the names of its columns and nothing else
 */
TEST_F(ServerGameStatisticsFixture, WritesTheNamesOfTheColumnsWhenItStarts) {
  start();

  EXPECT_EQ(fileText(), STATISTICS_HEADER_LINE);
}

/**
 * Given a game in progress where each pilot kills a Bydo
 * When a second of ticks passes
 * Then the file holds a row for each kill, naming the game, the pilot and the
 * points
 */
TEST_F(ServerGameStatisticsFixture, WritesARowForEachBydoKilledInAGame) {
  start();
  publish(FIRST_GAME, GAME_STARTED);
  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);
  publish(FIRST_GAME, KILL_BY_SECOND_PILOT);

  runTicks(TICKS_TO_A_WRITE);

  EXPECT_EQ(fileText(), std::string{STATISTICS_HEADER_LINE} +
                            std::string{FIRST_PILOT_KILL_IN_GAME_ONE} +
                            std::string{SECOND_PILOT_KILL_IN_GAME_ONE});
}

/**
 * Given a game where each pilot killed a Bydo
 * When the game ends and a second of ticks passes
 * Then the file ends with the total of the points of the game
 */
TEST_F(ServerGameStatisticsFixture, WritesTheTotalOfAGameThatEnds) {
  start();
  publish(FIRST_GAME, GAME_STARTED);
  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);
  publish(FIRST_GAME, KILL_BY_SECOND_PILOT);

  publish(FIRST_GAME, GAME_FINISHED);
  runTicks(TICKS_TO_A_WRITE);

  EXPECT_EQ(fileText(), std::string{STATISTICS_HEADER_LINE} +
                            std::string{FIRST_PILOT_KILL_IN_GAME_ONE} +
                            std::string{SECOND_PILOT_KILL_IN_GAME_ONE} +
                            std::string{TOTAL_OF_GAME_ONE});
}

/**
 * Given the second game started before the first
 * When a pilot kills a Bydo in each
 * Then the second game is game 1 and the first is game 2: games are numbered
 * in the order they start, not by where they run
 */
TEST_F(ServerGameStatisticsFixture, NumbersTheGamesInTheOrderTheyStart) {
  start();
  publish(SECOND_GAME, GAME_STARTED);
  publish(FIRST_GAME, GAME_STARTED);

  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);
  publish(SECOND_GAME, KILL_BY_SECOND_PILOT);
  runTicks(TICKS_TO_A_WRITE);

  EXPECT_EQ(fileText(), std::string{STATISTICS_HEADER_LINE} +
                            std::string{FIRST_PILOT_KILL_IN_GAME_TWO} +
                            std::string{SECOND_PILOT_KILL_IN_GAME_ONE});
}

/**
 * Given a game that started and ended without a kill
 * When it starts again and a pilot kills a Bydo
 * Then the kill belongs to game 2, after the empty total of game 1
 */
TEST_F(ServerGameStatisticsFixture, NumbersAGamePlayedAgainAsANewGame) {
  start();
  publish(FIRST_GAME, GAME_STARTED);
  publish(FIRST_GAME, GAME_FINISHED);

  publish(FIRST_GAME, GAME_STARTED);
  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);
  runTicks(TICKS_TO_A_WRITE);

  EXPECT_EQ(fileText(), std::string{STATISTICS_HEADER_LINE} +
                            std::string{EMPTY_TOTAL_OF_GAME_ONE} +
                            std::string{FIRST_PILOT_KILL_IN_GAME_TWO});
}

/**
 * Given a game waiting for its players
 * When a Bydo is killed and a second of ticks passes
 * Then the file holds no row: the kill belongs to no game
 */
TEST_F(ServerGameStatisticsFixture, LeavesOutABydoKilledOutsideAGame) {
  start();

  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);
  runTicks(TICKS_TO_A_WRITE);

  EXPECT_EQ(fileText(), STATISTICS_HEADER_LINE);
}

/**
 * Given a Bydo killed in a game
 * When 60 ticks pass, short of a second
 * Then its row is still in memory: the file holds the header only
 */
TEST_F(ServerGameStatisticsFixture, KeepsTheRowsInMemoryUntilASecondPassed) {
  start();
  publish(FIRST_GAME, GAME_STARTED);
  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);

  runTicks(TICKS_SHORT_OF_A_WRITE);

  EXPECT_EQ(fileText(), STATISTICS_HEADER_LINE);
}

/**
 * Given rows written once a second passed, and a Bydo killed after
 * When 60 more ticks pass
 * Then the new row waits: two writes are never less than a second apart
 */
TEST_F(ServerGameStatisticsFixture, WaitsAWholeSecondAgainAfterAWrite) {
  start();
  publish(FIRST_GAME, GAME_STARTED);
  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);
  runTicks(TICKS_TO_A_WRITE);
  publish(FIRST_GAME, KILL_BY_SECOND_PILOT);

  runTicks(TICKS_SHORT_OF_A_WRITE);

  EXPECT_EQ(fileText(), std::string{STATISTICS_HEADER_LINE} +
                            std::string{FIRST_PILOT_KILL_IN_GAME_ONE});
}

/**
 * Given a Bydo killed in a game, less than a second ago
 * When the server stops, as Ctrl-C stops it
 * Then the file holds its row
 */
TEST_F(ServerGameStatisticsFixture, WritesTheRowsLeftWhenTheServerStops) {
  start();
  publish(FIRST_GAME, GAME_STARTED);
  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);

  stop();

  EXPECT_EQ(fileText(), std::string{STATISTICS_HEADER_LINE} +
                            std::string{FIRST_PILOT_KILL_IN_GAME_ONE});
}

/**
 * Given a configuration that names no file
 * When the server starts and stops
 * Then the file is r-type_statistics_<port>.csv next to the executable
 */
TEST_F(ServerGameStatisticsFixture, NamesTheFileAfterThePortWithoutASetting) {
  const std::filesystem::path file =
      rtype::engine::executableFolder() / DEFAULT_STATISTICS_FILE_NAME;

  startWithTheDefaultFile();
  stop();

  EXPECT_TRUE(std::filesystem::exists(file));
  static_cast<void>(std::filesystem::remove(file));
}

/**
 * Given a file that is a folder
 * When the server starts
 * Then its start stops, naming the module
 */
TEST_F(ServerGameStatisticsFixture, StopsTheStartWhenTheFileCannotBeOpened) {
  EXPECT_THROW(startWritingTo(folder()), ModuleStartFailedException);
}

/**
 * Given a file on a device that refuses every write
 * When a Bydo is killed in each of two seconds
 * Then the journal says so once, and the game goes on
 */
TEST_F(ServerGameStatisticsFixture,
       TellsTheJournalOnceWhenTheFileCannotBeWritten) {
  if (!std::filesystem::exists(FULL_DISK_DEVICE)) {
    GTEST_SKIP() << "this system has no device that refuses every write";
  }
  startWritingTo(std::filesystem::path{FULL_DISK_DEVICE});
  publish(FIRST_GAME, GAME_STARTED);

  publish(FIRST_GAME, KILL_BY_FIRST_PILOT);
  runTicks(TICKS_TO_A_WRITE);
  publish(FIRST_GAME, KILL_BY_SECOND_PILOT);
  runTicks(TICKS_TO_A_WRITE);

  EXPECT_EQ(std::ranges::count_if(journal().lines(),
                                  [](const RecordedLine& line) {
                                    return line.level ==
                                           rtype::logging::LogLevel::Warn;
                                  }),
            1);
}
