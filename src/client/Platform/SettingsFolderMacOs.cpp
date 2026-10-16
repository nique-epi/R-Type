#include <sys/syslimits.h>
#include <sysdir.h>
#include <array>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include "PlatformConstants.hpp"
#include "SettingsFolder.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> settingsFolder() {
  std::array<char, PATH_MAX> reported{};
  const sysdir_search_path_enumeration_state search =
      sysdir_start_search_path_enumeration(SYSDIR_DIRECTORY_APPLICATION_SUPPORT,
                                           SYSDIR_DOMAIN_MASK_USER);
  if (sysdir_get_next_search_path_enumeration(search, reported.data()) == 0) {
    return std::nullopt;
  }
  std::string folder(reported.data());
  if (folder.starts_with(HOME_SHORTHAND)) {
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    const char* const home = std::getenv(HOME_VARIABLE);
    if (home == nullptr || std::string(home).empty()) {
      return std::nullopt;
    }
    folder.replace(0, 1, home);
  }
  return std::filesystem::path(folder) / MACOS_SETTINGS_FOLDER_NAME;
}

}  // namespace rtype::client
