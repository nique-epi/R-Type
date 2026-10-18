#include <cstdlib>
#include <filesystem>
#include <optional>
#include "PlatformConstants.hpp"
#include "SettingsFolder.hpp"

namespace rtype::client {

namespace {

/** @returns The value of an environment variable when it is an absolute
 * path, nothing otherwise. */
std::optional<std::filesystem::path> absolutePathIn(const char* variable) {
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const char* const value = std::getenv(variable);
  if (value == nullptr || !std::filesystem::path(value).is_absolute()) {
    return std::nullopt;
  }
  return std::filesystem::path(value);
}

}  // namespace

std::optional<std::filesystem::path> settingsFolder() {
  const std::optional<std::filesystem::path> configuration =
      absolutePathIn(XDG_CONFIG_HOME_VARIABLE);
  if (configuration.has_value()) {
    return *configuration / LINUX_SETTINGS_FOLDER_NAME;
  }
  const std::optional<std::filesystem::path> home =
      absolutePathIn(HOME_VARIABLE);
  if (!home.has_value()) {
    return std::nullopt;
  }
  return *home / LINUX_CONFIGURATION_FOLDER / LINUX_SETTINGS_FOLDER_NAME;
}

}  // namespace rtype::client
