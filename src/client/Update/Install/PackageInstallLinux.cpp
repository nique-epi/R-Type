#include <filesystem>
#include <optional>
#include <system_error>
#include "InstallConstants.hpp"
#include "InstallPlace.hpp"
#include "LaunchCommand.hpp"
#include "PackageInstall.hpp"

namespace rtype::client {

namespace {

std::filesystem::path partialFileOf(const InstallPlace& place) {
  std::filesystem::path partial = place.installed;
  partial += PARTIAL_INSTALL_SUFFIX;
  return partial;
}

}  // namespace

std::optional<LaunchCommand> installPackage(
    const std::filesystem::path& package, const InstallPlace& place) {
  const std::filesystem::path partial = partialFileOf(place);
  std::error_code error;
  std::filesystem::copy_file(package, partial,
                             std::filesystem::copy_options::overwrite_existing,
                             error);
  if (!error) {
    std::filesystem::permissions(partial, APPIMAGE_PERMISSIONS, error);
  }
  if (!error) {
    std::filesystem::rename(partial, place.installed, error);
  }
  if (error) {
    removeLeftovers(place);
    return std::nullopt;
  }
  return LaunchCommand{.program = place.installed, .arguments = {}};
}

void removeLeftovers(const InstallPlace& place) {
  std::error_code ignored;
  std::filesystem::remove(partialFileOf(place), ignored);
}

}  // namespace rtype::client
