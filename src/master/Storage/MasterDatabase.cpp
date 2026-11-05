#include "MasterDatabase.hpp"
#include <SQLiteCpp/Database.h>
#include <mutex>
#include <string>
#include <utility>
#include "DatabaseFile.hpp"
#include "MasterStorageConstants.hpp"

namespace rtype::master {

void MasterDatabase::open(const std::string& file) {
  SQLite::Database opened = storage::openDatabaseFile(file, MASTER_MIGRATIONS);
  const std::scoped_lock lock{mutex_};
  connection_.emplace(std::move(opened));
}

void MasterDatabase::close() {
  const std::scoped_lock lock{mutex_};
  connection_.reset();
}

}  // namespace rtype::master
