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
  const std::filesystem::path parentFolder = executableFolder.parent_path();
  const std::vector<std::filesystem::path> candidates{
      executableFolder / ASSETS_FOLDER_NAME, parentFolder / ASSETS_FOLDER_NAME,
      parentFolder / APP_RESOURCES_FOLDER_NAME / ASSETS_FOLDER_NAME};
  for (const std::filesystem::path& candidate : candidates) {
    std::error_code error;
    if (std::filesystem::is_directory(candidate, error)) {
      return candidate;
    }
  }
  std::vector<std::string> searchedFolders;
  std::ranges::transform(candidates, std::back_inserter(searchedFolders),
                         logging::genericText);
  throw AssetFolderNotFoundException(searchedFolders);
}

}  // namespace rtype::engine
