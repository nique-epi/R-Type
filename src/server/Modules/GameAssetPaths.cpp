#include "GameAssetPaths.hpp"
#include <filesystem>
#include "AssetFolder.hpp"
#include "EntitySheetConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

std::filesystem::path wavesFileInAssets() {
  return engine::locateAssetFolder() / WAVE_FILE_IN_ASSETS;
}

std::filesystem::path entitySheetsFolderInAssets() {
  return engine::locateAssetFolder() / game::ENTITY_SHEET_FOLDER_IN_ASSETS;
}

}  // namespace rtype::server
