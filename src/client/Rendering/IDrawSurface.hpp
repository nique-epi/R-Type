#pragma once

#include <string_view>
#include "Position.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Where the render system draws. It knows nothing about entities.
 */
class IDrawSurface {
 public:
  virtual ~IDrawSurface() = default;

  /**
   * @param assetId Id of the image to draw.
   * @param frame Rectangle of the image to show, in pixels of the image.
   * @param center Center of the rectangle, in logical units of the playfield.
   */
  virtual void draw(std::string_view assetId, game::SpriteFrame frame,
                    game::Position center) = 0;
};

}  // namespace rtype::client
