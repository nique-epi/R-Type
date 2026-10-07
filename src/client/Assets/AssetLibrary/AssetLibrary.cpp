#include "AssetLibrary.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Exception.hpp>
#include <algorithm>
#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include "AssetIds.hpp"
#include "AssetKind.hpp"
#include "AssetList.hpp"
#include "AssetLoadResult.hpp"
#include "ClientException.hpp"

namespace rtype::client {

namespace {

template <typename Asset>
AssetLoadResult loadInto(AssetsById<Asset>& assets, std::string_view assetId,
                         const std::filesystem::path& file) {
  if (assets.contains(assetId)) {
    return AssetLoadResult::Loaded;
  }
  std::error_code statusError;
  if (!std::filesystem::is_regular_file(file, statusError)) {
    return AssetLoadResult::MissingFile;
  }
  try {
    assets.try_emplace(std::string(assetId), file);
  } catch (const sf::Exception&) {
    return AssetLoadResult::UnreadableFile;
  }
  return AssetLoadResult::Loaded;
}

bool isNamedIn(const std::vector<std::string>& assetIds,
               std::string_view assetId) {
  return std::ranges::find(assetIds, assetId) != assetIds.end();
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

template <typename Asset>
const Asset& loadedAsset(const AssetsById<Asset>& assets,
                         std::string_view assetId) {
  const auto found = assets.find(assetId);
  if (found == assets.end()) {
    throw AssetNotLoadedException(assetId);
  }
  return found->second;
}

}  // namespace

AssetLibrary::AssetLibrary(std::filesystem::path folder)
    : folder_(std::move(folder)) {}

const std::filesystem::path& AssetLibrary::folder() const { return folder_; }

std::filesystem::path AssetLibrary::fileOf(std::string_view assetId) const {
  return folder_ / std::filesystem::path(assetId);
}

AssetLoadResult AssetLibrary::load(AssetKind kind, std::string_view assetId) {
  if (!isValidAssetId(assetId)) {
    return AssetLoadResult::InvalidId;
  }
  const std::filesystem::path file = fileOf(assetId);
  switch (kind) {
    case AssetKind::Texture:
      return loadInto(textures_, assetId, file);
    case AssetKind::Sound:
      return loadInto(sounds_, assetId, file);
    case AssetKind::Font:
      return loadInto(fonts_, assetId, file);
    case AssetKind::Music:
      return loadInto(musicFiles_, assetId, file);
  }
  return AssetLoadResult::UnreadableFile;
}

bool AssetLibrary::isLoaded(AssetKind kind, std::string_view assetId) const {
  switch (kind) {
    case AssetKind::Texture:
      return textures_.contains(assetId);
    case AssetKind::Sound:
      return sounds_.contains(assetId);
    case AssetKind::Font:
      return fonts_.contains(assetId);
    case AssetKind::Music:
      return musicFiles_.contains(assetId);
  }
  return false;
}

void AssetLibrary::keepPermanently(const AssetList& permanent) {
  appendAll(permanent_.textures, permanent.textures);
  appendAll(permanent_.sounds, permanent.sounds);
  appendAll(permanent_.fonts, permanent.fonts);
  appendAll(permanent_.music, permanent.music);
}

void AssetLibrary::releaseAllExcept(const AssetList& kept) {
  keepOnly(textures_, kept.textures, permanent_.textures);
  keepOnly(sounds_, kept.sounds, permanent_.sounds);
  keepOnly(fonts_, kept.fonts, permanent_.fonts);
  keepOnly(musicFiles_, kept.music, permanent_.music);
}

const sf::Texture& AssetLibrary::texture(std::string_view assetId) const {
  return loadedAsset(textures_, assetId);
}

const sf::SoundBuffer& AssetLibrary::sound(std::string_view assetId) const {
  return loadedAsset(sounds_, assetId);
}

const sf::Font& AssetLibrary::font(std::string_view assetId) const {
  return loadedAsset(fonts_, assetId);
}

const std::filesystem::path& AssetLibrary::musicFile(
    std::string_view assetId) const {
  return loadedAsset(musicFiles_, assetId);
}

}  // namespace rtype::client
