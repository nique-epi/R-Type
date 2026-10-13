#pragma once

#include <string>
#include "Layer.hpp"
#include "SpriteFrame.hpp"

namespace rtype::game {

/**
 * @brief How an entity looks: the asset to draw, the rectangle of it to show
 * and the layer it is drawn in.
 *
 * The asset is named by its id; turning the id into an image is the business
 * of the client only.
 */
struct Sprite {
  std::string assetId;
  SpriteFrame frame;
  Layer layer;
};

}  // namespace rtype::game
