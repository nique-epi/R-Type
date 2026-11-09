#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace rtype::client {

/** @brief Where this client was installed from a package of a release. */
struct InstallPlace {
  /** @brief What an update replaces: the AppImage, the R-Type.app bundle, or
   * the folder the Windows installer wrote. */
  std::filesystem::path installed;
  /** @brief Key of the package of this system in the manifest. */
  std::string system;
};

/** @returns Where this client is installed, or nothing when it runs from a
 * build folder, an archive, or an app macOS translocated. */
[[nodiscard]] std::optional<InstallPlace> installPlace();

}  // namespace rtype::client
