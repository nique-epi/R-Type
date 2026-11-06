#include <filesystem>
#include <optional>
#include "JournalFolder.hpp"
#include "MacOsUserDirectory.hpp"
#include "MacOsUserFolder.hpp"
#include "PlatformConstants.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> journalFolder() {
  return macOsUserFolder(MacOsUserDirectory::Library, MACOS_LOGS_FOLDER_NAME);
}

}  // namespace rtype::client
