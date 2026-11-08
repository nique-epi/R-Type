#include "DatabaseLocation.hpp"
#include <filesystem>
#include <utility>
#include "IListeningPort.hpp"
#include "ServerDatabaseFile.hpp"
#include "SettingValue.hpp"

namespace rtype::server {

DatabaseLocation::DatabaseLocation(const engine::SettingValue& setting,
                                   std::filesystem::path folder,
                                   const IListeningPort& port)
    : setting_(setting), folder_(std::move(folder)), port_(port) {}

std::filesystem::path DatabaseLocation::file() const {
  return databaseFileOf(setting_.get().text(), folder_,
                        port_.get().listeningPort());
}

}  // namespace rtype::server
