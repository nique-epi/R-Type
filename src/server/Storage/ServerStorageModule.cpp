#include "ServerStorageModule.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "IListeningPort.hpp"
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "PathText.hpp"
#include "ResultsArchiveConstants.hpp"
#include "ServerConstants.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::server {

ServerStorageModule::ServerStorageModule(std::filesystem::path defaultFolder)
    : defaultFolder_(std::move(defaultFolder)),
      logger_(std::string{STORAGE_LOGGER_NAME}) {}

std::string ServerStorageModule::name() const {
  return std::string{STORAGE_MODULE_NAME};
}

std::vector<std::string> ServerStorageModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ServerStorageModule::install(engine::InstallContext& context) {
  databaseSetting_ =
      &context.require<engine::ConfigurationRegistry>().declareText(
          engine::TextSettingDefinition{
              .name = std::string{DATABASE_SETTING_NAME},
              .defaultValue = {},
              .help = "SQLite file the leaderboard of this server is kept "
                      "in; empty for the file of its port next to the "
                      "executable",
              .validate = {}});
  listeningPort_ = context.find<IListeningPort>();
  context.provide<IResultsArchive>(archive_);
}

void ServerStorageModule::start() {
  const std::filesystem::path file = databaseFile();
  archive_.open(file);
  logger_.info("results database ", engine::genericText(file));
}

void ServerStorageModule::stop() { archive_.close(); }

std::filesystem::path ServerStorageModule::databaseFile() const {
  if (!databaseSetting_->text().empty()) {
    return std::filesystem::path{databaseSetting_->text()};
  }
  const std::uint16_t port = listeningPort_ != nullptr
                                 ? listeningPort_->listeningPort()
                                 : network::DEFAULT_SERVER_PORT;
  return defaultFolder_ /
         (std::string{DATABASE_FILE_PREFIX} + std::to_string(port) +
          std::string{DATABASE_FILE_SUFFIX});
}

}  // namespace rtype::server
