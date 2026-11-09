#include <filesystem>
#include <optional>
#include <string_view>
#include <system_error>
#include "ChildProcess.hpp"
#include "InstallConstants.hpp"
#include "InstallPlace.hpp"
#include "LaunchCommand.hpp"
#include "PackageInstall.hpp"

namespace rtype::client {

namespace {

std::filesystem::path withSuffix(const std::filesystem::path& path,
                                 std::string_view suffix) {
  std::filesystem::path result = path;
  result += suffix;
  return result;
}

/** @returns Whether the bundle of the disk image @p package was copied to
 * @p staged; the image is detached either way. */
bool copyBundleOut(const std::filesystem::path& package,
                   const std::filesystem::path& staged) {
  const std::filesystem::path volume =
      package.parent_path() / DISK_IMAGE_MOUNT_FOLDER;
  if (runToEnd(LaunchCommand{
          .program = DISK_IMAGE_TOOL,
          .arguments = {"attach", "-nobrowse", "-readonly", "-noautoopen",
                        "-mountpoint", volume, package}}) != 0) {
    return false;
  }
  const bool copied =
      runToEnd(
          LaunchCommand{.program = COPY_TOOL,
                        .arguments = {volume / APP_BUNDLE_NAME, staged}}) == 0;
  static_cast<void>(runToEnd(LaunchCommand{
      .program = DISK_IMAGE_TOOL, .arguments = {"detach", volume, "-force"}}));
  return copied;
}

/** @returns Whether @p staged now lies at @p installed, the bundle it replaces
 * kept beside it until the next start; on a failure, the installed bundle is
 * put back. */
bool swapBundle(const std::filesystem::path& staged,
                const std::filesystem::path& installed) {
  const std::filesystem::path replaced =
      withSuffix(installed, REPLACED_BUNDLE_SUFFIX);
  std::error_code error;
  std::filesystem::remove_all(replaced, error);
  std::filesystem::rename(installed, replaced, error);
  if (error) {
    return false;
  }
  std::filesystem::rename(staged, installed, error);
  if (error) {
    std::error_code ignored;
    std::filesystem::rename(replaced, installed, ignored);
    return false;
  }
  return true;
}

}  // namespace

std::optional<LaunchCommand> installPackage(
    const std::filesystem::path& package, const InstallPlace& place) {
  const std::filesystem::path staged =
      withSuffix(place.installed, STAGED_BUNDLE_SUFFIX);
  std::error_code ignored;
  std::filesystem::remove_all(staged, ignored);
  if (!copyBundleOut(package, staged) || !swapBundle(staged, place.installed)) {
    std::filesystem::remove_all(staged, ignored);
    return std::nullopt;
  }
  return LaunchCommand{
      .program = OPEN_TOOL,
      .arguments = {OPEN_NEW_INSTANCE_OPTION, place.installed}};
}

void removeLeftovers(const InstallPlace& place) {
  std::error_code ignored;
  std::filesystem::remove_all(
      withSuffix(place.installed, REPLACED_BUNDLE_SUFFIX), ignored);
  std::filesystem::remove_all(withSuffix(place.installed, STAGED_BUNDLE_SUFFIX),
                              ignored);
}

}  // namespace rtype::client
