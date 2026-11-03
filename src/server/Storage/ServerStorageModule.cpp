#include "ServerStorageModule.hpp"
#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "ResultsArchiveConstants.hpp"

namespace rtype::server {

ServerStorageModule::ServerStorageModule(std::filesystem::path databaseFile)
    : databaseFile_(std::move(databaseFile)),
      logger_(std::string{STORAGE_LOGGER_NAME}) {}

std::string ServerStorageModule::name() const {
  return std::string{STORAGE_MODULE_NAME};
}

std::vector<std::string> ServerStorageModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ServerStorageModule::install(engine::InstallContext& context) {
  context.provide<IResultsArchive>(archive_);
}

void ServerStorageModule::start() {
  archive_.open(databaseFile_);
  logger_.info("results database ", engine::genericText(databaseFile_));
}

void ServerStorageModule::stop() { archive_.close(); }

}  // namespace rtype::server
