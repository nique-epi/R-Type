#include <sysdir.h>
#include <filesystem>
#include <optional>
#include "MacOsUserFolder.hpp"
#include "PlatformConstants.hpp"
#include "SettingsFolder.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> settingsFolder() {
  return macOsUserFolder(SYSDIR_DIRECTORY_APPLICATION_SUPPORT,
                         MACOS_SETTINGS_FOLDER_NAME);
}

}  // namespace rtype::client
