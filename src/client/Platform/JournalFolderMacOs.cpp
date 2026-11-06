#include <sysdir.h>
#include <filesystem>
#include <optional>
#include "JournalFolder.hpp"
#include "MacOsUserFolder.hpp"
#include "PlatformConstants.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> journalFolder() {
  return macOsUserFolder(SYSDIR_DIRECTORY_LIBRARY, MACOS_LOGS_FOLDER_NAME);
}

}  // namespace rtype::client
