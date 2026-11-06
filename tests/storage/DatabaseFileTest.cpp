#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <gtest/gtest.h>
#include <string>
#include "DatabaseFile.hpp"
#include "DatabaseFileFixture.hpp"
#include "DatabaseFileTestConstants.hpp"
#include "StorageException.hpp"

namespace rtype::storage::testing {

/**
 * Given a file that does not exist
 * When it is opened with two migrations
 * Then it holds version 2 and what the migrations did
 */
TEST_F(DatabaseFileFixture, CreatesTheFileAndAppliesEveryMigration) {
  SQLite::Database database = openDatabaseFile(file(), SECOND_VERSION);

  EXPECT_EQ(numberFrom(database, READ_USER_VERSION), 2);
  EXPECT_EQ(numberFrom(database, COUNT_SCORES), 1);
}

/**
 * Given a file at version 1
 * When it is opened with two migrations
 * Then only the second one runs: the first would refuse an existing table
 */
TEST_F(DatabaseFileFixture, AppliesOnlyTheMigrationsTheFileDoesNotHold) {
  {
    const SQLite::Database first = openDatabaseFile(file(), FIRST_VERSION);
  }

  SQLite::Database second = openDatabaseFile(file(), SECOND_VERSION);

  EXPECT_EQ(numberFrom(second, READ_USER_VERSION), 2);
  EXPECT_EQ(numberFrom(second, COUNT_SCORES), 1);
}

/**
 * Given a file at version 2
 * When it is opened again with the same migrations
 * Then nothing runs twice: one row, not two
 */
TEST_F(DatabaseFileFixture, RunsNoMigrationTwice) {
  {
    const SQLite::Database first = openDatabaseFile(file(), SECOND_VERSION);
  }

  SQLite::Database again = openDatabaseFile(file(), SECOND_VERSION);

  EXPECT_EQ(numberFrom(again, COUNT_SCORES), 1);
}

/**
 * Given a migration that writes a row, then fails
 * When the file is opened
 * Then the opening fails, and the file is left at version 1 without that row
 */
TEST_F(DatabaseFileFixture, LeavesNothingOfAMigrationThatFails) {
  EXPECT_THROW((void)openDatabaseFile(file(), SECOND_VERSION_FAILS),
               UnopenableDatabaseException);

  SQLite::Database database = openDatabaseFile(file(), FIRST_VERSION);
  EXPECT_EQ(numberFrom(database, READ_USER_VERSION), 1);
  EXPECT_EQ(numberFrom(database, COUNT_SCORES), 0);
}

/**
 * Given a file at version 2
 * When a program that knows one migration opens it
 * Then it refuses the file
 */
TEST_F(DatabaseFileFixture, RefusesAFileFromANewerProgram) {
  {
    const SQLite::Database newer = openDatabaseFile(file(), SECOND_VERSION);
  }

  EXPECT_THROW((void)openDatabaseFile(file(), FIRST_VERSION),
               NewerDatabaseException);
}

/**
 * Given a new file
 * When it is opened
 * Then the connection runs in WAL with synchronous FULL and foreign keys on
 */
TEST_F(DatabaseFileFixture, RunsWithTheSettingsThatSurviveACrash) {
  SQLite::Database database = openDatabaseFile(file(), FIRST_VERSION);

  EXPECT_EQ(database.execAndGet(std::string{READ_JOURNAL_MODE}).getString(),
            "wal");
  EXPECT_EQ(numberFrom(database, READ_SYNCHRONOUS), SYNCHRONOUS_FULL_VALUE);
  EXPECT_EQ(numberFrom(database, READ_FOREIGN_KEYS), 1);
}

/**
 * Given a new file another connection is writing to, as a second game started
 * at the same time does
 * When it is opened
 * Then the opening waits for that write and switches the file to WAL
 */
TEST_F(DatabaseFileFixture, WaitsForAnotherWriterBeforeSwitchingToWal) {
  SQLite::Database database =
      openWhileAnotherConnectionWrites(NO_MIGRATION, CREATE_SCORES);

  EXPECT_EQ(database.execAndGet(std::string{READ_JOURNAL_MODE}).getString(),
            "wal");
}

/**
 * Given a file at version 0 another connection is migrating to version 1
 * When it is opened with that migration
 * Then the opening finds version 1 once that write ends, and runs nothing again
 */
TEST_F(DatabaseFileFixture, RunsNoMigrationAnotherConnectionApplied) {
  {
    const SQLite::Database empty = openDatabaseFile(file(), NO_MIGRATION);
  }

  SQLite::Database database =
      openWhileAnotherConnectionWrites(FIRST_VERSION, MIGRATE_TO_FIRST_VERSION);

  EXPECT_EQ(numberFrom(database, READ_USER_VERSION), 1);
}

/**
 * Given a database that cannot use WAL, one held in memory
 * When it is opened
 * Then the opening fails instead of running without durability
 */
TEST_F(DatabaseFileFixture, RefusesADatabaseThatCannotUseWal) {
  EXPECT_THROW(
      (void)openDatabaseFile(std::string{MEMORY_DATABASE}, FIRST_VERSION),
      JournalModeRefusedException);
}

/**
 * Given a file in a folder that does not exist
 * When it is opened
 * Then the opening fails
 */
TEST_F(DatabaseFileFixture, RefusesAFileItCannotCreate) {
  EXPECT_THROW((void)openDatabaseFile(fileInAMissingFolder(), FIRST_VERSION),
               UnopenableDatabaseException);
}

}  // namespace rtype::storage::testing
