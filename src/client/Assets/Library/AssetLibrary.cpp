#include "AssetLibrary.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <algorithm>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AssetKind.hpp"
#include "AssetList.hpp"
#include "AssetLoadResult.hpp"
#include "AssetsById.hpp"

namespace rtype::client {

namespace {

bool isNamedIn(const std::vector<std::string>& assetIds,
               std::string_view assetId) {
  return std::ranges::find(assetIds, assetId) != assetIds.end();
}

template <typename Asset>
bool holdsEvery(const AssetsById<Asset>& assets,
                const std::vector<std::string>& assetIds) {
  return std::ranges::all_of(assetIds, [&assets](const std::string& assetId) {
    return assets.contains(assetId);
  });
}

template <typename Asset>
void keepOnly(AssetsById<Asset>& assets,
              const std::vector<std::string>& keptIds,
              const std::vector<std::string>& permanentIds) {
  std::erase_if(assets, [&keptIds, &permanentIds](const auto& entry) {
    return !isNamedIn(keptIds, entry.first) &&
           !isNamedIn(permanentIds, entry.first);
  });
}

void appendAll(std::vector<std::string>& assetIds,
               const std::vector<std::string>& addedIds) {
  assetIds.insert(assetIds.end(), addedIds.begin(), addedIds.end());
}

}  // namespace

AssetLibrary::AssetLibrary(std::filesystem::path folder)
    : folder_(std::move(folder)) {}

const std::filesystem::path& AssetLibrary::folder() const { return folder_; }

std::filesystem::path AssetLibrary::fileOf(std::string_view assetId) const {
  return folder_ / std::filesystem::path(assetId);
}

AssetLoadResult AssetLibrary::load(AssetKind kind, std::string_view assetId) {
  const std::filesystem::path file = fileOf(assetId);
  switch (kind) {
    case AssetKind::Texture:
      return loadAssetFile(textures_, assetId, file);
    case AssetKind::Font:
      return loadAssetFile(fonts_, assetId, file);
    case AssetKind::Music:
      return loadAssetFile(musicFiles_, assetId, file);
  }
  return AssetLoadResult::UnreadableFile;
}

bool AssetLibrary::isLoaded(AssetKind kind, std::string_view assetId) const {
  switch (kind) {
    case AssetKind::Texture:
      return textures_.contains(assetId);
    case AssetKind::Font:
      return fonts_.contains(assetId);
    case AssetKind::Music:
      return musicFiles_.contains(assetId);
  }
  return false;
}

bool AssetLibrary::isLoaded(const AssetList& assets) const {
  return holdsEvery(textures_, assets.textures) &&
         holdsEvery(fonts_, assets.fonts) &&
         holdsEvery(musicFiles_, assets.music);
}

void AssetLibrary::keepPermanently(const AssetList& permanent) {
  appendAll(permanent_.textures, permanent.textures);
  appendAll(permanent_.fonts, permanent.fonts);
  appendAll(permanent_.music, permanent.music);
}

void AssetLibrary::releaseAllExcept(const AssetList& kept) {
  keepOnly(textures_, kept.textures, permanent_.textures);
  keepOnly(fonts_, kept.fonts, permanent_.fonts);
  keepOnly(musicFiles_, kept.music, permanent_.music);
}

const sf::Texture& AssetLibrary::texture(std::string_view assetId) const {
  return loadedAsset(textures_, assetId);
}

const sf::Texture* AssetLibrary::findTexture(std::string_view assetId) const {
  const auto found = textures_.find(assetId);
  return found == textures_.end() ? nullptr : &found->second;
}

const sf::Font& AssetLibrary::font(std::string_view assetId) const {
  return loadedAsset(fonts_, assetId);
}

const std::filesystem::path& AssetLibrary::musicFile(
    std::string_view assetId) const {
  return loadedAsset(musicFiles_, assetId);
}

}  // namespace rtype::client
