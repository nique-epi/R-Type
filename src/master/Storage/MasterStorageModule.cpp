#include "MasterStorageModule.hpp"
#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "InstallContext.hpp"
#include "MasterConstants.hpp"
#include "MasterModuleConstants.hpp"
#include "PathText.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::master {

MasterStorageModule::MasterStorageModule(std::filesystem::path defaultFolder)
    : defaultFolder_(std::move(defaultFolder)),
      logger_(std::string{MASTER_LOGGER_NAME}) {}

std::string MasterStorageModule::name() const {
  return std::string{STORAGE_MODULE_NAME};
}

std::vector<std::string> MasterStorageModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void MasterStorageModule::install(engine::InstallContext& context) {
  databaseSetting_ =
      &context.require<engine::ConfigurationRegistry>().declareText(
          engine::TextSettingDefinition{
              .name = std::string{DATABASE_SETTING_NAME},
              .defaultValue = {},
              .help = "SQLite file the master keeps its data in, on a local "
                      "disk; empty for " +
                      std::string{DATABASE_FILE_NAME} +
                      " next to the executable",
              .validate = {}});
  context.provide(database_);
}

void MasterStorageModule::start() {
  const std::filesystem::path file = databaseFile();
  database_.open(logging::genericText(file));
  logger_.info("database ", logging::genericText(file));
}

void MasterStorageModule::stop() { database_.close(); }

std::filesystem::path MasterStorageModule::databaseFile() const {
  if (!databaseSetting_->text().empty()) {
    return std::filesystem::path{databaseSetting_->text()};
  }
  return defaultFolder_ / DATABASE_FILE_NAME;
}

}  // namespace rtype::master
