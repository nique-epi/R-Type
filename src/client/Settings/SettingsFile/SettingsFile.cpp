#include "SettingsFile.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include "AssetIds.hpp"
#include "PlayerSettings.hpp"
#include "SettingsConstants.hpp"
#include "SettingsText.hpp"

namespace rtype::client {

SettingsFile::SettingsFile(std::optional<std::filesystem::path> path)
    : path_(std::move(path)), logger_(std::string(SETTINGS_LOG_MODULE_NAME)) {}

PlayerSettings SettingsFile::load(const PlayerSettings& defaults) const {
  if (!path_.has_value()) {
    return defaults;
  }
  std::error_code error;
  const std::uintmax_t size = std::filesystem::file_size(*path_, error);
  if (error) {
    logger_.info("no settings file at ", genericText(*path_),
                 ", first launch settings used");
    return defaults;
  }
  if (size > MAXIMUM_SETTINGS_FILE_SIZE) {
    logger_.warn("settings file too large, ignored: ", genericText(*path_));
    return defaults;
  }
  std::ifstream file(*path_, std::ios::binary);
  const std::string text((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
  if (!file.good() && !file.eof()) {
    logger_.warn("settings file unreadable, ignored: ", genericText(*path_));
    return defaults;
  }
  const SettingsReading reading = readSettingsText(text, defaults);
  for (const std::string& line : reading.ignoredLines) {
    logger_.warn("settings file ", genericText(*path_), ", ", line);
  }
  return reading.settings;
}

bool SettingsFile::save(const PlayerSettings& settings) {
  if (!path_.has_value()) {
    return false;
  }
  std::error_code error;
  const std::filesystem::path folder = path_->parent_path();
  if (std::filesystem::create_directories(folder, error)) {
    std::filesystem::permissions(folder, std::filesystem::perms::owner_all,
                                 error);
  }
  if (error) {
    logger_.warn("cannot create the settings folder ", genericText(folder),
                 ": ", error.message());
    return false;
  }
  std::filesystem::path temporary = *path_;
  temporary += TEMPORARY_SETTINGS_SUFFIX;
  if (!write(temporary, settings)) {
    logger_.warn("cannot write ", genericText(temporary));
    std::filesystem::remove(temporary, error);
    return false;
  }
  std::filesystem::rename(temporary, *path_, error);
  if (error) {
    logger_.warn("cannot replace ", genericText(*path_), ": ", error.message());
    std::filesystem::remove(temporary, error);
    return false;
  }
  return true;
}

bool SettingsFile::write(const std::filesystem::path& file,
                         const PlayerSettings& settings) {
  std::ofstream stream(file, std::ios::binary | std::ios::trunc);
  stream << settingsText(settings);
  stream.close();
  return !stream.fail();
}

}  // namespace rtype::client
