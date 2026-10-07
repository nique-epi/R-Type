#include "AssetFolder.hpp"
#include <algorithm>
#include <filesystem>
#include <iterator>
#include <optional>
#include <string>
#include <system_error>
#include <vector>
#include "AssetConstants.hpp"
#include "AssetIds.hpp"
#include "ClientException.hpp"
#include "ExecutablePath.hpp"

namespace rtype::client {

std::filesystem::path locateAssetFolder() {
  const std::optional<std::filesystem::path> executable = executablePath();
  if (!executable.has_value()) {
    throw UnknownExecutablePathException();
  }
  return findAssetFolder(executable->parent_path());
}

std::filesystem::path findAssetFolder(
    const std::filesystem::path& executableFolder) {
  const std::vector<std::filesystem::path> candidates{
      executableFolder / ASSETS_FOLDER_NAME,
      executableFolder.parent_path() / ASSETS_FOLDER_NAME};
  for (const std::filesystem::path& candidate : candidates) {
    std::error_code error;
    if (std::filesystem::is_directory(candidate, error)) {
      return candidate;
    }
  }
  std::vector<std::string> searchedFolders;
  std::ranges::transform(candidates, std::back_inserter(searchedFolders),
                         genericText);
  throw AssetFolderNotFoundException(searchedFolders);
}

}  // namespace rtype::client
