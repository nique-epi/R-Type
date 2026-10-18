#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include "IDrawSurface.hpp"
#include "ITextureSource.hpp"
#include "Logger.hpp"
#include "Position.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"

namespace rtype::client {

/**
 * @brief Draws rectangles of images on the canvas of a PixelCanvas.
 *
 * A position in logical units becomes a pixel of the canvas, which covers
 * BACKGROUND_PIXEL_SIZE logical units, and every rectangle lands on whole
 * pixels, so it is copied pixel for pixel, like the stars of the background.
 * The target and the texture source must outlive it. An id the source does not
 * know is not drawn and is reported once in the log.
 *
 * A flash is drawn with FLASH_FRAGMENT_SHADER, compiled at construction; when
 * the graphics card has no shaders or the shader does not compile, a flash is
 * drawn twice, the second time added onto the first, which brightens it, and
 * the log says so once.
 */
class SfmlDrawSurface : public IDrawSurface {
 public:
  /** @param canvas Canvas of a PixelCanvas, whose view is in its own pixels. */
  SfmlDrawSurface(sf::RenderTarget& canvas, const ITextureSource& textures);

  void draw(std::string_view assetId, game::SpriteFrame frame,
            game::Position center, SpriteTone tone) override;

 private:
  /** @brief Draws a sprite placed on the canvas as a flash: a silhouette in
   * FLASH_COLOR, or the sprite brightened when there is no shader. */
  void drawFlash(sf::Sprite& sprite);

  sf::RenderTarget& canvas_;
  const ITextureSource& textures_;
  logging::Logger logger_;
  std::set<std::string, std::less<>> reportedMissingIds_;
  std::optional<sf::Shader> flashShader_;
};

}  // namespace rtype::client
