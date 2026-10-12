#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What an asset file holds, which decides how it is loaded. */
enum class AssetKind : std::uint8_t {
  Texture,
  Sound,
  Font,
  Music,
};

}  // namespace rtype::client
