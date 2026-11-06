#include <filesystem>
#include <optional>
#include "MacOsUserDirectory.hpp"
#include "MacOsUserFolder.hpp"
#include "PlatformConstants.hpp"
#include "SettingsFolder.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> settingsFolder() {
  return macOsUserFolder(MacOsUserDirectory::ApplicationSupport,
                         MACOS_SETTINGS_FOLDER_NAME);
}

}  // namespace rtype::client
