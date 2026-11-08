#include "ServerStorageModule.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "DatabaseLocation.hpp"
#include "IListeningPort.hpp"
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "ResultsArchiveConstants.hpp"
#include "ServerConstants.hpp"
#include "SettingValue.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::server {

namespace {

/** @returns The database setting, declared in @p configuration. */
const engine::SettingValue& declareDatabase(
    engine::ConfigurationRegistry& configuration) {
  return configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{DATABASE_SETTING_NAME},
      .defaultValue = {},
      .help = "SQLite file the leaderboard of this server is kept in; empty "
              "for the file of its port next to the executable",
      .validate = {}});
}

}  // namespace

ServerStorageModule::ServerStorageModule(std::filesystem::path defaultFolder)
    : defaultFolder_(std::move(defaultFolder)),
      logger_(std::string{STORAGE_LOGGER_NAME}) {}

std::string ServerStorageModule::name() const {
  return std::string{STORAGE_MODULE_NAME};
}

std::vector<std::string> ServerStorageModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{NETWORK_MODULE_NAME}};
}

void ServerStorageModule::install(engine::InstallContext& context) {
  location_.emplace(
      declareDatabase(context.require<engine::ConfigurationRegistry>()),
      defaultFolder_, context.require<IListeningPort>());
  context.provide<IResultsArchive>(archive_);
}

void ServerStorageModule::start() {
  const std::filesystem::path file = location_->file();
  archive_.open(file);
  logger_.info("results database ", logging::genericText(file));
}

void ServerStorageModule::stop() { archive_.close(); }

}  // namespace rtype::server
