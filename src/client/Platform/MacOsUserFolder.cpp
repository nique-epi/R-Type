#include "MacOsUserFolder.hpp"
#include <sys/syslimits.h>
#include <sysdir.h>
#include <array>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include "MacOsUserDirectory.hpp"
#include "PlatformConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The sysdir name of @p directory. */
sysdir_search_path_directory_t sysdirNameOf(MacOsUserDirectory directory) {
  switch (directory) {
    case MacOsUserDirectory::Library:
      return SYSDIR_DIRECTORY_LIBRARY;
    case MacOsUserDirectory::ApplicationSupport:
      return SYSDIR_DIRECTORY_APPLICATION_SUPPORT;
  }
  return SYSDIR_DIRECTORY_LIBRARY;
}

}  // namespace

std::optional<std::filesystem::path> macOsUserFolder(
    MacOsUserDirectory directory, std::string_view child) {
  std::array<char, PATH_MAX> reported{};
  const sysdir_search_path_enumeration_state search =
      sysdir_start_search_path_enumeration(sysdirNameOf(directory),
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
  return std::filesystem::path(folder) / child;
}

}  // namespace rtype::client
