#include "GameAssetPaths.hpp"
#include <filesystem>
#include <span>
#include "ContentFolders.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

std::filesystem::path wavesFileOf(
    std::span<const std::filesystem::path> contentFolders) {
  return engine::topmostContentFile(contentFolders,
                                    std::filesystem::path{WAVE_FILE_IN_ASSETS});
}

}  // namespace rtype::server
