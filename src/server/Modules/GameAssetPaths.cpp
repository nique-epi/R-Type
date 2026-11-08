#include "GameAssetPaths.hpp"
#include <filesystem>
#include <span>
#include <vector>
#include "ContentFolders.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

std::vector<std::filesystem::path> wavesFilesOf(
    std::span<const std::filesystem::path> contentFolders) {
  return engine::contentFilesOf(contentFolders,
                                std::filesystem::path{WAVE_FILE_IN_ASSETS});
}

}  // namespace rtype::server
