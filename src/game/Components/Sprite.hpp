#pragma once

#include <string>
#include "Layer.hpp"

namespace rtype::game {

/**
 * @brief How an entity looks: the asset to draw and the layer it is drawn in.
 *
 * The asset is named by its id; turning the id into an image is the business
 * of the client only.
 */
struct Sprite {
  std::string assetId;
  Layer layer;
};

}  // namespace rtype::game
