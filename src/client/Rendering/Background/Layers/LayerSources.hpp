#pragma once

#include <cstdint>
#include "AssetFileLookup.hpp"

namespace rtype::client {

/** @brief What the layers of a background are built from, besides their
 * description: where the files they name lie, and the seed of their random
 * draws. */
struct LayerSources {
  /** Finds the file an asset id names; it must not be empty. */
  AssetFileLookup fileOf;
  std::uint32_t seed{};
};

}  // namespace rtype::client
