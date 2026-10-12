#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How the loading of one asset went. */
enum class AssetLoadResult : std::uint8_t {
  Loaded,
  InvalidId,
  MissingFile,
  UnreadableFile,
};

}  // namespace rtype::client
