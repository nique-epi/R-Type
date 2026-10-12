#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <set>
#include <string>
#include <string_view>
#include "IDrawSurface.hpp"
#include "ITextureSource.hpp"
#include "Logger.hpp"
#include "Position.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Draws rectangles of images on the canvas of a PixelCanvas.
 *
 * A position in logical units becomes a pixel of the canvas, which covers
 * BACKGROUND_PIXEL_SIZE logical units, and every rectangle lands on whole
 * pixels, so it is copied pixel for pixel, like the stars of the background.
 * The target and the texture source must outlive it. An id the source does not
 * know is not drawn and is reported once in the log.
 */
class SfmlDrawSurface : public IDrawSurface {
 public:
  /** @param canvas Canvas of a PixelCanvas, whose view is in its own pixels. */
  SfmlDrawSurface(sf::RenderTarget& canvas, const ITextureSource& textures);

  void draw(std::string_view assetId, game::SpriteFrame frame,
            game::Position center) override;

 private:
  sf::RenderTarget& canvas_;
  const ITextureSource& textures_;
  logging::Logger logger_;
  std::set<std::string, std::less<>> reportedMissingIds_;
};

}  // namespace rtype::client
