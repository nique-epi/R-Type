#include "DatabaseFile.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Exception.h>
#include <SQLiteCpp/Transaction.h>
#include <sqlite3.h>
#include <chrono>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include "StorageConstants.hpp"
#include "StorageException.hpp"

namespace rtype::storage {

int readWriteOrCreate() {
  return static_cast<int>(static_cast<unsigned int>(SQLite::OPEN_READWRITE) |
                          static_cast<unsigned int>(SQLite::OPEN_CREATE));
}

namespace {

/**
 * @brief Sets what every connection runs with. SQLite answers the mode in force
 * rather than failing when it cannot change it, so the answer is read.
 * @throws JournalModeRefusedException when that mode is not WAL.
 */
/**
 * @brief Asks for WAL, again while another connection holds the lock the
 * switch needs, until DATABASE_BUSY_TIMEOUT_MILLISECONDS have passed.
 *
 * The switch reads the file, then promotes its read lock to rewrite it. When
 * another connection already holds a write lock, as a second game opening the
 * same new file does, waiting could deadlock, so SQLite answers SQLITE_BUSY at
 * once instead of calling the busy handler (sqlite3_busy_handler()).
 * @returns The mode SQLite answers.
 */
std::string switchToWal(SQLite::Database& database) {
  const auto deadline =
      std::chrono::steady_clock::now() +
      std::chrono::milliseconds{DATABASE_BUSY_TIMEOUT_MILLISECONDS};
  while (true) {
    try {
      return database.execAndGet(std::string{ENABLE_WAL_PRAGMA}).getString();
    } catch (const SQLite::Exception& error) {
      if (error.getErrorCode() != SQLITE_BUSY ||
          std::chrono::steady_clock::now() >= deadline) {
        throw;
      }
    }
    std::this_thread::sleep_for(WAL_SWITCH_RETRY_PAUSE);
  }
}

void applySafeWrites(SQLite::Database& database, const std::string& fileText) {
  const std::string mode = switchToWal(database);
  if (mode != WAL_JOURNAL_MODE) {
    throw JournalModeRefusedException(fileText, mode);
  }
  database.exec(std::string{SYNCHRONOUS_FULL_PRAGMA});
  database.exec(std::string{FOREIGN_KEYS_ON_PRAGMA});
}

/**
 * @returns The version @p database holds.
 * @throws NewerDatabaseException when that version is above the count of
 * @p migrations.
 */
std::size_t versionHeld(SQLite::Database& database, const std::string& fileText,
                        std::span<const std::string_view> migrations) {
  const int found =
      database.execAndGet(std::string{USER_VERSION_PRAGMA}).getInt();
  if (std::cmp_greater(found, migrations.size())) {
    throw NewerDatabaseException(fileText, found);
  }
  return static_cast<std::size_t>(found);
}

/** @brief Applies @p migration and writes @p version. */
void writeMigration(SQLite::Database& database, std::string_view migration,
                    std::size_t version) {
  database.exec(std::string{migration});
  database.exec(std::string{SET_USER_VERSION_PRAGMA} + std::to_string(version));
}

/**
 * @brief Applies the migration after the version the file holds, in one
 * transaction that reads that version under its write lock, so a migration
 * another connection committed meanwhile is never applied again.
 * @returns Whether a migration was applied.
 */
bool applyNextMigration(SQLite::Database& database, const std::string& fileText,
                        std::span<const std::string_view> migrations) {
  SQLite::Transaction transaction{database,
                                  SQLite::TransactionBehavior::IMMEDIATE};
  const std::size_t version = versionHeld(database, fileText, migrations);
  if (version == migrations.size()) {
    return false;
  }
  writeMigration(database, migrations[version], version + 1);
  transaction.commit();
  return true;
}

/**
 * @brief Applies the migrations above the version the file holds. A file that
 * holds them all takes no write lock.
 * @throws NewerDatabaseException when that version is above their count.
 */
void applyMigrations(SQLite::Database& database, const std::string& fileText,
                     std::span<const std::string_view> migrations) {
  if (versionHeld(database, fileText, migrations) == migrations.size()) {
    return;
  }
  while (applyNextMigration(database, fileText, migrations)) {
  }
}

}  // namespace

SQLite::Database openDatabaseFile(
    const std::string& file, std::span<const std::string_view> migrations) {
  try {
    SQLite::Database database{file, readWriteOrCreate(),
                              DATABASE_BUSY_TIMEOUT_MILLISECONDS};
    applySafeWrites(database, file);
    applyMigrations(database, file, migrations);
    return database;
  } catch (const SQLite::Exception& error) {
    throw UnopenableDatabaseException(file, error.what());
  }
}

}  // namespace rtype::storage
