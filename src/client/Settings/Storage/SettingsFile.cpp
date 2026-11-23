#include "SettingsFile.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include "PathText.hpp"
#include "PlayerFile.hpp"
#include "PlayerFileWrite.hpp"
#include "PlayerSettings.hpp"
#include "SettingsConstants.hpp"
#include "SettingsText.hpp"

namespace rtype::client {

SettingsFile::SettingsFile(std::optional<std::filesystem::path> path)
    : path_(std::move(path)), logger_(std::string(SETTINGS_LOGGER_NAME)) {}

PlayerSettings SettingsFile::load(const PlayerSettings& defaults) const {
  if (!path_.has_value()) {
    return defaults;
  }
  const std::optional<std::string> text =
      readPlayerFile(*path_, MAXIMUM_SETTINGS_FILE_SIZE, logger_);
  if (!text.has_value()) {
    logger_.info("no settings read from ", logging::genericText(*path_),
                 ", first launch settings used");
    return defaults;
  }
  const SettingsReading reading = readSettingsText(*text, defaults);
  for (const std::string& line : reading.ignoredLines) {
    logger_.warn("settings file ", logging::genericText(*path_), ", ", line);
  }
  return reading.settings;
}

bool SettingsFile::save(const PlayerSettings& settings) {
  if (!path_.has_value()) {
    return false;
  }
  const std::string text = settingsText(settings);
  return replacePlayerFile(
      PlayerFileWrite{.file = *path_, .text = text, .permissions = {}},
      logger_);
}

}  // namespace rtype::client
