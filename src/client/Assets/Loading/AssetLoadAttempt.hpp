#pragma once

#include <filesystem>
#include <string>
#include "AssetLoadResult.hpp"

namespace rtype::client {

/** @brief What one load of an asset gave: the asset id, the file it names in
 * the assets folder, and how the load went. */
struct AssetLoadAttempt {
  std::string assetId;
  std::filesystem::path file;
  AssetLoadResult result{};
};

}  // namespace rtype::client
