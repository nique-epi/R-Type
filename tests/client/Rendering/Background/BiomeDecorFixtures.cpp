#include "BiomeDecorFixtures.hpp"
#include <filesystem>
#include <string_view>
#include "AssetFileLookup.hpp"
#include "BiomeDecorTestConstants.hpp"
#include "TextureLookup.hpp"

rtype::client::TextureLookup noTexture() {
  return [](std::string_view) { return nullptr; };
}

rtype::client::AssetFileLookup filesOfTheAssets() {
  return [](std::string_view assetId) {
    return std::filesystem::path(RTYPE_SOURCE_ASSETS_FOLDER) /
           std::filesystem::path(assetId);
  };
}

rtype::client::AssetFileLookup filesNowhere() {
  return [](std::string_view) { return std::filesystem::path(FILE_NOWHERE); };
}
