#include "ClientSettingsModule.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "ClientConstants.hpp"
#include "HighContrast.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "PlayerSettings.hpp"
#include "ReducedMotion.hpp"
#include "SettingsConstants.hpp"
#include "SettingsFolder.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

namespace {

/** @returns The settings of a first launch, flashes reduced when the system
 * asks for less motion, and high contrast on when it asks for more contrast. */
PlayerSettings firstLaunchSettings() {
  PlayerSettings settings;
  settings.reduceFlashes = systemPrefersReducedMotion();
  settings.highContrast = systemPrefersHighContrast();
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
      service_(settings_, storage_),
      logger_(std::string{CLIENT_LOGGER_NAME}) {}

std::string ClientSettingsModule::name() const {
  return std::string{SETTINGS_MODULE_NAME};
}

std::vector<std::string> ClientSettingsModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ClientSettingsModule::install(engine::InstallContext& context) {
  context.provide(settings_);
  context.provide(service_);
}

void ClientSettingsModule::start() {
  logFileLocation();
  service_.restore(storage_.load(firstLaunchSettings()));
}

void ClientSettingsModule::stop() {}

void ClientSettingsModule::logFileLocation() const {
  if (!file_.has_value()) {
    logger_.warn("no settings folder, the settings last for this launch only");
    return;
  }
  logger_.info("settings file: ", logging::genericText(*file_));
}

}  // namespace rtype::client
