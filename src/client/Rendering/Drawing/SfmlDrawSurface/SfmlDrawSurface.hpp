#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include "IDrawSurface.hpp"
#include "ITextureSource.hpp"
#include "Logger.hpp"
#include "Position.hpp"
#include "SpriteDrawing.hpp"
#include "SpriteTone.hpp"
#include "TextStyle.hpp"
#include "TextWriter.hpp"

namespace rtype::client {

/**
 * @brief Draws rectangles of images and lines of text on the canvas of a
 * PixelCanvas.
 *
 * A position in logical units becomes a pixel of the canvas, which covers
 * BACKGROUND_PIXEL_SIZE logical units, and every rectangle lands on whole
 * pixels, so it is copied pixel for pixel, like the stars of the background.
 * The target, the texture source and the text writer must outlive it. An id
 * the source does not know is not drawn and is reported once in the log.
 *
 * A flash and an outline are drawn with FLASH_FRAGMENT_SHADER, a ghost with
 * GHOST_FRAGMENT_SHADER, both compiled at construction. Where the graphics
 * card has no shaders or a shader does not compile, the log says so once: a
 * flash is drawn twice, the second time added onto the first, which brightens
 * it; a ghost is drawn half transparent; an outline is left out.
 */
class SfmlDrawSurface : public IDrawSurface {
 public:
  /**
   * @param canvas Canvas of a PixelCanvas, whose view is in its own pixels.
   * @param textures Where the images are found.
   * @param writer Writes the lines of text.
   */
  SfmlDrawSurface(sf::RenderTarget& canvas, const ITextureSource& textures,
                  const TextWriter& writer);

  void draw(const SpriteDrawing& drawing) override;
  void write(std::string_view text, game::Position center,
             TextStyle style) override;

 private:
  /** @returns The texture of an id, or nullptr once the log says it is
   * missing; a missing id is reported once. */
  [[nodiscard]] const sf::Texture* foundTexture(std::string_view assetId);

  /** @returns The rectangle of the drawing as a sprite, its top-left corner
   * on the whole pixel nearest to where the center puts it. */
  [[nodiscard]] static sf::Sprite placedSprite(const sf::Texture& texture,
                                               const SpriteDrawing& drawing);

  /** @brief Draws a placed sprite in its tone. */
  void drawToned(sf::Sprite& sprite, SpriteTone tone);

  /** @brief Draws a sprite placed on the canvas as a flash: a silhouette in
   * FLASH_COLOR, or the sprite brightened when there is no shader. */
  void drawFlash(sf::Sprite& sprite);

  /** @brief Draws a sprite placed on the canvas dithered, or half transparent
   * when there is no shader. */
  void drawGhost(sf::Sprite& sprite);

  /** @brief Draws the silhouette of a placed sprite in @p color one pixel
   * away in each of the four CROSS_DIRECTIONS; nothing without the flash
   * shader. */
  void drawOutline(const sf::Sprite& sprite, sf::Color color);

  sf::RenderTarget& canvas_;
  const ITextureSource& textures_;
  const TextWriter& writer_;
  logging::Logger logger_;
  std::set<std::string, std::less<>> reportedMissingIds_;
  std::optional<sf::Shader> flashShader_;
  std::optional<sf::Shader> ghostShader_;
};

}  // namespace rtype::client
