#include "ServerDatabaseFile.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include "ServerConstants.hpp"

namespace rtype::server {

std::filesystem::path databaseFileOf(std::string_view setting,
                                     const std::filesystem::path& folder,
                                     std::uint16_t port) {
  if (!setting.empty()) {
    return std::filesystem::path{setting};
  }
  return folder / (std::string{DATABASE_FILE_PREFIX} + std::to_string(port) +
                   std::string{DATABASE_FILE_SUFFIX});
}

}  // namespace rtype::server
