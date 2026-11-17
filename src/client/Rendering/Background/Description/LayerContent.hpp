#pragma once

#include <variant>
#include "ImageContent.hpp"
#include "MapContent.hpp"
#include "ParticleContent.hpp"

namespace rtype::client {

/** @brief What a layer of a background shows: an image, a layer of an LDtk
 * project, tiles or props, or particles. */
using LayerContent = std::variant<ImageContent, MapContent, ParticleContent>;

}  // namespace rtype::client
