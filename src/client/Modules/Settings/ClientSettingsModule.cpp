#include "ClientSettingsModule.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "AssetIds.hpp"
#include "ClientConstants.hpp"
#include "ISettingsStorage.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PlayerSettings.hpp"
#include "ReducedMotion.hpp"
#include "SettingsConstants.hpp"
#include "SettingsFolder.hpp"

namespace rtype::client {

namespace {

/** @returns The settings of a first launch, flashes reduced when the system
 * asks for less motion. */
PlayerSettings firstLaunchSettings() {
  PlayerSettings settings;
  settings.reduceFlashes = systemPrefersReducedMotion();
  return settings;
}

/** @returns The file the settings are kept in, or nothing when the system
 * tells no settings folder. */
std::optional<std::filesystem::path> settingsFileLocation() {
  const std::optional<std::filesystem::path> folder = settingsFolder();
  if (!folder.has_value()) {
    return std::nullopt;
  }
  return *folder / SETTINGS_FILE_NAME;
}

}  // namespace

ClientSettingsModule::ClientSettingsModule()
    : file_(settingsFileLocation()),
      storage_(file_),
      logger_(std::string{CLIENT_LOGGER_NAME}) {}

std::string ClientSettingsModule::name() const {
  return std::string{SETTINGS_MODULE_NAME};
}

std::vector<std::string> ClientSettingsModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ClientSettingsModule::install(engine::InstallContext& context) {
  context.provide(settings_);
  context.provide<ISettingsStorage>(storage_);
}

void ClientSettingsModule::start() {
  logFileLocation();
  settings_ = storage_.load(firstLaunchSettings());
}

void ClientSettingsModule::stop() {}

void ClientSettingsModule::logFileLocation() const {
  if (!file_.has_value()) {
    logger_.warn("no settings folder, the settings last for this launch only");
    return;
  }
  logger_.info("settings file: ", genericText(*file_));
}

}  // namespace rtype::client
