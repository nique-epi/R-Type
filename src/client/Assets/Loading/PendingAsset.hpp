#pragma once

#include <string>
#include "AssetKind.hpp"

namespace rtype::client {

/** @brief An asset a loading step still has to load. */
struct PendingAsset {
  AssetKind kind{};
  std::string assetId;
};

}  // namespace rtype::client
