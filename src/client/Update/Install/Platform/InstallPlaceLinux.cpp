#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include "InstallConstants.hpp"
#include "InstallPlace.hpp"

namespace rtype::client {

std::optional<InstallPlace> installPlace() {
  const char* const appImage = std::getenv(APPIMAGE_VARIABLE);
  if (appImage == nullptr || !std::filesystem::path(appImage).is_absolute()) {
    return std::nullopt;
  }
  return InstallPlace{.installed = std::filesystem::path(appImage),
                      .system = std::string{LINUX_PACKAGE_SYSTEM}};
}

}  // namespace rtype::client
