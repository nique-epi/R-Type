#pragma once

#include <filesystem>

namespace rtype::server {

/** @returns The waves file the games play, WAVE_FILE_IN_ASSETS in the assets
 * folder locateAssetFolder() finds next to the executable. */
[[nodiscard]] std::filesystem::path wavesFileInAssets();

/** @returns The folder of the entity sheets the games are made from,
 * ENTITY_SHEET_FOLDER_IN_ASSETS in the same assets folder. */
[[nodiscard]] std::filesystem::path entitySheetsFolderInAssets();

}  // namespace rtype::server
