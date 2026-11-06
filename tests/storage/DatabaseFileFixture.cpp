#include "DatabaseFileFixture.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <future>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include "DatabaseFile.hpp"
#include "DatabaseFileTestConstants.hpp"
#include "TemporaryFolder.hpp"

namespace {

/** @returns The flags that open a file for writing and create it when it does
 * not exist; SQLiteCpp declares each as a signed value. */
int readWriteOrCreate() {
  return static_cast<int>(static_cast<unsigned int>(SQLite::OPEN_READWRITE) |
                          static_cast<unsigned int>(SQLite::OPEN_CREATE));
}

}  // namespace

std::string DatabaseFileFixture::file() const {
  return (folder_.path() / rtype::storage::testing::DATABASE_FILE_NAME)
      .string();
}

std::string DatabaseFileFixture::fileInAMissingFolder() const {
  return (folder_.path() / rtype::storage::testing::MISSING_FOLDER_FILE)
      .string();
}

int DatabaseFileFixture::numberFrom(SQLite::Database& database,
                                    std::string_view query) {
  return database.execAndGet(std::string{query}).getInt();
}

SQLite::Database DatabaseFileFixture::openWhileAnotherConnectionWrites(
    std::span<const std::string_view> migrations,
    std::string_view write) const {
  SQLite::Database other{file(), readWriteOrCreate()};
  other.exec(std::string{rtype::storage::testing::BEGIN_IMMEDIATE});
  other.exec(std::string{write});
  std::future<SQLite::Database> opening =
      std::async(std::launch::async, [this, migrations] {
        return rtype::storage::openDatabaseFile(file(), migrations);
      });
  std::this_thread::sleep_for(rtype::storage::testing::WRITE_HELD_FOR);
  other.exec(std::string{rtype::storage::testing::COMMIT});
  return opening.get();
}
