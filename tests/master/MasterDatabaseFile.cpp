#include "MasterDatabaseFile.hpp"
#include <filesystem>
#include "MasterDatabase.hpp"
#include "MasterTestConstants.hpp"

MasterDatabaseFile::MasterDatabaseFile() { database_.open(file().string()); }

rtype::master::MasterDatabase& MasterDatabaseFile::database() {
  return database_;
}

std::filesystem::path MasterDatabaseFile::file() const {
  return folder_.path() / TEST_DATABASE_FILE;
}

void MasterDatabaseFile::restart() {
  database_.close();
  database_.open(file().string());
}

void MasterDatabaseFile::close() { database_.close(); }
