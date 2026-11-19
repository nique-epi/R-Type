#include "MapPreviews.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetLoadResult.hpp"
#include "ISessionPack.hpp"
#include "PathText.hpp"

namespace rtype::client {

MapPreviews::MapPreviews(AssetLibrary& assets, const ISessionPack& files)
    : assets_(&assets), files_(&files) {}

std::optional<std::string> MapPreviews::previewOf(std::string_view path) {
  const std::optional<std::filesystem::path> file = files_->packFileOf(path);
  if (!file.has_value()) {
    return std::nullopt;
  }
  std::string asset = logging::genericText(*file);
  if (unloadable_.contains(asset)) {
    return std::nullopt;
  }
  if (assets_->load(AssetKind::Texture, asset) != AssetLoadResult::Loaded) {
    unloadable_.insert(asset);
    return std::nullopt;
  }
  return asset;
}

}  // namespace rtype::client
