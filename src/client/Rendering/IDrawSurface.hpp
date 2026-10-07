#pragma once

#include <string_view>
#include "Position.hpp"

namespace rtype::client {

/**
 * @brief Where the render system draws. It knows nothing about entities.
 */
class IDrawSurface {
 public:
  virtual ~IDrawSurface() = default;

  /**
   * @param assetId Id of the image to draw.
   * @param center Center of the image, in logical units of the playfield.
   */
  virtual void draw(std::string_view assetId, game::Position center) = 0;
};

}  // namespace rtype::client
