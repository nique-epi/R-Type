#include <SQLiteCpp/Database.h>
#include <gtest/gtest.h>
#include <array>
#include <string>
#include "DatabaseFile.hpp"
#include "ResultsDatabase.hpp"
#include "ResultsDatabaseConstants.hpp"
#include "ResultsDatabaseFixture.hpp"
#include "ServerMigrationsConstants.hpp"
#include "ServerMigrationsTestConstants.hpp"
#include "StorageConstants.hpp"

namespace rtype::server::testing {

/**
 * Given a file of the third version holding a ban, as a server built when the
 * console banned pilots left it
 * When the server opens it
 * Then the file no longer holds the table of the bans
 */
TEST_F(ResultsDatabaseFixture, DropsTheBansOfAFileOfTheThirdVersion) {
  {
    SQLite::Database thirdVersion = storage::openDatabaseFile(
        file().string(),
        std::array{RESULTS_TABLE_MIGRATION, UNSENT_GAMES_MIGRATION,
                   BANS_TABLE_MIGRATION});
    thirdVersion.exec(std::string{INSERT_BAN_OF_ACE});
  }
  const ResultsDatabase database{file()};

  SQLite::Database reader{file().string(), SQLite::OPEN_READONLY,
                          storage::DATABASE_BUSY_TIMEOUT_MILLISECONDS};
  EXPECT_EQ(reader.execAndGet(std::string{COUNT_BANS_TABLES}).getInt(), 0);
}

/**
 * Given no file
 * When the server creates its file
 * Then the file holds no table of the bans
 */
TEST_F(ResultsDatabaseFixture, CreatesAFileWithoutTheBans) {
  const ResultsDatabase database{file()};

  SQLite::Database reader{file().string(), SQLite::OPEN_READONLY,
                          storage::DATABASE_BUSY_TIMEOUT_MILLISECONDS};
  EXPECT_EQ(reader.execAndGet(std::string{COUNT_BANS_TABLES}).getInt(), 0);
}

}  // namespace rtype::server::testing
