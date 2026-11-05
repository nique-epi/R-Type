#include "MasterDatabaseFixture.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include "DatabaseFile.hpp"
#include "MasterDatabase.hpp"
#include "MasterDatabaseTestConstants.hpp"
#include "MasterStorageConstants.hpp"
#include "MasterTestConstants.hpp"

void MasterDatabaseFixture::keepVersionOneServer(
    std::string_view lastHeartbeat) const {
  const SQLite::Database versionOne = rtype::storage::openDatabaseFile(
      file().string(), std::span{rtype::master::MASTER_MIGRATIONS}.first(
                           VERSION_ONE_MIGRATIONS));
  SQLite::Statement insert{versionOne, std::string{INSERT_VERSION_ONE_SERVER}};
  insert.bind(std::string{VERSION_ONE_HEARTBEAT_PARAMETER},
              std::string{lastHeartbeat});
  insert.bind(
      std::string{VERSION_ONE_DATE_PARAMETER},
      static_cast<std::int64_t>(VERSION_ONE_DATE.time_since_epoch().count()));
  insert.exec();
}

rtype::master::MasterDatabase& MasterDatabaseFixture::upgradedDatabase() {
  database_.open(file().string());
  return database_;
}

std::filesystem::path MasterDatabaseFixture::file() const {
  return folder_.path() / TEST_DATABASE_FILE;
}
