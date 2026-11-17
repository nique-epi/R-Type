#pragma once

#include <variant>
#include "ImageContent.hpp"
#include "ParticleContent.hpp"
#include "PropContent.hpp"
#include "TileContent.hpp"

namespace rtype::client {

/** @brief What a layer of a background shows: one of the four kinds of
 * content. */
using LayerContent =
    std::variant<ImageContent, TileContent, ParticleContent, PropContent>;

}  // namespace rtype::client
