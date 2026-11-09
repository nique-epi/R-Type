#include <filesystem>
#include <optional>
#include <string>
#include <system_error>
#include "ExecutablePath.hpp"
#include "InstallConstants.hpp"
#include "InstallPlace.hpp"

namespace rtype::client {

std::optional<InstallPlace> installPlace() {
  const std::optional<std::filesystem::path> executable =
      engine::executablePath();
  if (!executable.has_value()) {
    return std::nullopt;
  }
  const std::filesystem::path installed =
      executable->parent_path().parent_path();
  std::error_code error;
  if (!std::filesystem::exists(installed / WINDOWS_UNINSTALLER_NAME, error)) {
    return std::nullopt;
  }
  return InstallPlace{.installed = installed,
                      .system = std::string{WINDOWS_PACKAGE_SYSTEM}};
}

}  // namespace rtype::client
