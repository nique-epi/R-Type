#include "DatabaseFileFixture.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <string>
#include <string_view>
#include "DatabaseFileTestConstants.hpp"
#include "TemporaryFolder.hpp"

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
