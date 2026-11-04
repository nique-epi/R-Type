#pragma once

#include <SFML/System/Exception.hpp>
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <system_error>
#include "AssetIds.hpp"
#include "AssetLoadResult.hpp"
#include "ClientException.hpp"

namespace rtype::client {

/** @brief Assets of one kind, found by asset id. */
template <typename Asset>
using AssetsById = std::map<std::string, Asset, std::less<>>;

/**
 * @brief Reads @p file into @p assets under @p assetId, unless the id breaks
 * the asset id rule or is loaded already; an Asset is built from the path of
 * its file, and SFML throws sf::Exception when it cannot read it.
 *
 * @returns Loaded when the asset is held, read now or before; InvalidId,
 * MissingFile or UnreadableFile otherwise, with nothing added. It never
 * throws.
 */
template <typename Asset>
[[nodiscard]] AssetLoadResult loadAssetFile(AssetsById<Asset>& assets,
                                            std::string_view assetId,
                                            const std::filesystem::path& file) {
  if (!isValidAssetId(assetId)) {
    return AssetLoadResult::InvalidId;
  }
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

/**
 * @returns The asset @p assetId names in @p assets.
 * @throws AssetNotLoadedException when it names none.
 */
template <typename Asset>
[[nodiscard]] const Asset& loadedAsset(const AssetsById<Asset>& assets,
                                       std::string_view assetId) {
  const auto found = assets.find(assetId);
  if (found == assets.end()) {
    throw AssetNotLoadedException(assetId);
  }
  return found->second;
}

}  // namespace rtype::client
