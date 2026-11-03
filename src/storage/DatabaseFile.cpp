#include "DatabaseFile.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Exception.h>
#include <SQLiteCpp/Transaction.h>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "StorageConstants.hpp"
#include "StorageException.hpp"

namespace rtype::storage {

namespace {

/** @returns The flags that open a file for writing and create it when it does
 * not exist; SQLiteCpp declares each as a signed value. */
int readWriteOrCreate() {
  return static_cast<int>(static_cast<unsigned int>(SQLite::OPEN_READWRITE) |
                          static_cast<unsigned int>(SQLite::OPEN_CREATE));
}

/**
 * @brief Sets what every connection runs with. SQLite answers the mode in force
 * rather than failing when it cannot change it, so the answer is read.
 * @throws JournalModeRefusedException when that mode is not WAL.
 */
void applySafeWrites(SQLite::Database& database, const std::string& fileText) {
  const std::string mode =
      database.execAndGet(std::string{ENABLE_WAL_PRAGMA}).getString();
  if (mode != WAL_JOURNAL_MODE) {
    throw JournalModeRefusedException(fileText, mode);
  }
  database.exec(std::string{SYNCHRONOUS_FULL_PRAGMA});
  database.exec(std::string{FOREIGN_KEYS_ON_PRAGMA});
}

/** @brief Applies @p migration and writes @p version, in one transaction. */
void applyMigration(SQLite::Database& database, std::string_view migration,
                    std::size_t version) {
  SQLite::Transaction transaction{database,
                                  SQLite::TransactionBehavior::IMMEDIATE};
  database.exec(std::string{migration});
  database.exec(std::string{SET_USER_VERSION_PRAGMA} + std::to_string(version));
  transaction.commit();
}

/**
 * @brief Applies the migrations above the version the file holds.
 * @throws NewerDatabaseException when that version is above their count.
 */
void applyMigrations(SQLite::Database& database, const std::string& fileText,
                     std::span<const std::string_view> migrations) {
  const int found =
      database.execAndGet(std::string{USER_VERSION_PRAGMA}).getInt();
  if (std::cmp_greater(found, migrations.size())) {
    throw NewerDatabaseException(fileText, found);
  }
  for (auto version = static_cast<std::size_t>(found);
       version < migrations.size(); ++version) {
    applyMigration(database, migrations[version], version + 1);
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
