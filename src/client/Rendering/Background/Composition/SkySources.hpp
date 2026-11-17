#pragma once

#include <cstdint>
#include "AssetFileLookup.hpp"
#include "TextureLookup.hpp"

namespace rtype::client {

/** @brief What the sky draws its backgrounds from, besides their
 * descriptions: the seed of its random draws, the images the client has
 * loaded, and where the files a layer names lie. Neither lookup may be
 * empty. */
struct SkySources {
  std::uint32_t seed{};
  TextureLookup textureOf;
  AssetFileLookup fileOf;
};

}  // namespace rtype::client
