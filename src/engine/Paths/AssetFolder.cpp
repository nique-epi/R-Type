#include "AssetFolder.hpp"
#include <algorithm>
#include <filesystem>
#include <iterator>
#include <string>
#include <system_error>
#include <vector>
#include "EngineException.hpp"
#include "ExecutablePath.hpp"
#include "PathConstants.hpp"
#include "PathText.hpp"

namespace rtype::engine {

std::filesystem::path locateAssetFolder() {
  return findAssetFolder(executableFolder());
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

}  // namespace rtype::engine
