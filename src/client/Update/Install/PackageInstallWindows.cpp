#include <filesystem>
#include <optional>
#include "InstallConstants.hpp"
#include "InstallPlace.hpp"
#include "LaunchCommand.hpp"
#include "PackageInstall.hpp"

namespace rtype::client {

std::optional<LaunchCommand> installPackage(
    const std::filesystem::path& package, const InstallPlace& place) {
  std::filesystem::path folder{INSTALL_FOLDER_OPTION};
  folder += place.installed.native();
  return LaunchCommand{.program = package,
                       .arguments = {SILENT_INSTALL_OPTION, folder}};
}

void removeLeftovers([[maybe_unused]] const InstallPlace& place) {}

}  // namespace rtype::client
