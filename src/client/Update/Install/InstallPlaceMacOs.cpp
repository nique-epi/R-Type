#include <filesystem>
#include <optional>
#include <string>
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
  const std::filesystem::path folder = executable->parent_path();
  const std::filesystem::path bundle = folder.parent_path().parent_path();
  if (folder.filename() != MACOS_EXECUTABLE_FOLDER ||
      bundle.extension() != APP_BUNDLE_EXTENSION ||
      bundle.native().find(TRANSLOCATION_FOLDER) != std::string::npos) {
    return std::nullopt;
  }
  return InstallPlace{.installed = bundle,
                      .system = std::string{MACOS_PACKAGE_SYSTEM}};
}

}  // namespace rtype::client
