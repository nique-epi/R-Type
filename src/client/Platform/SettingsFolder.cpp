#include "SettingsFolder.hpp"
#include <filesystem>
#include <optional>
#include <string_view>

namespace rtype::client {

std::optional<std::filesystem::path> settingsFolderFile(std::string_view name) {
  const std::optional<std::filesystem::path> folder = settingsFolder();
  if (!folder.has_value()) {
    return std::nullopt;
  }
  return *folder / name;
}

}  // namespace rtype::client
