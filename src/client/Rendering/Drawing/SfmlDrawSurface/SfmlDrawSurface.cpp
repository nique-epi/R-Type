#include "SfmlDrawSurface.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <string_view>
#include "BackgroundConstants.hpp"
#include "EntitySprites.hpp"
#include "FlashShaderConstants.hpp"
#include "FragmentShader.hpp"
#include "GhostShaderConstants.hpp"
#include "ITextureSource.hpp"
#include "InterfaceConstants.hpp"
#include "PixelRounding.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "SpriteDrawing.hpp"
#include "SpriteTone.hpp"
#include "SpriteToneColorConstants.hpp"
#include "TextLine.hpp"
#include "TextStyle.hpp"
#include "TextWriter.hpp"

namespace rtype::client {

SfmlDrawSurface::SfmlDrawSurface(sf::RenderTarget& canvas,
                                 const ITextureSource& textures,
                                 const TextWriter& writer)
    : canvas_(canvas),
      textures_(textures),
      writer_(writer),
      logger_(std::string(LOG_MODULE_NAME)),
      flashShader_(loadFragmentShader(FLASH_SHADER, logger_)),
      ghostShader_(loadFragmentShader(GHOST_SHADER, logger_)) {}

void SfmlDrawSurface::draw(const SpriteDrawing& drawing) {
  const sf::Texture* texture = foundTexture(drawing.assetId);
  if (texture == nullptr) {
    return;
  }
  sf::Sprite sprite = placedSprite(*texture, drawing);
  if (drawing.outline.has_value()) {
    drawOutline(sprite, *drawing.outline);
  }
  drawToned(sprite, drawing.tone);
}

void SfmlDrawSurface::write(std::string_view text, game::Position center,
                            TextStyle style) {
  const sf::Vector2i middle =
      nearestPixel(sf::Vector2f(center.x, center.y) / BACKGROUND_PIXEL_SIZE);
  style.alignment = TextAlignment::Center;
  writer_.write(
      canvas_,
      TextLine{.text = text,
               .anchor = {middle.x,
                          middle.y - static_cast<int>(TEXT_LINE_HEIGHT * HALF)},
               .style = style});
}

const sf::Texture* SfmlDrawSurface::foundTexture(std::string_view assetId) {
  const sf::Texture* texture = textures_.texture(assetId);
  if (texture == nullptr && reportedMissingIds_.emplace(assetId).second) {
    logger_.warn("no texture for asset id ", assetId);
  }
  return texture;
}

sf::Sprite SfmlDrawSurface::placedSprite(const sf::Texture& texture,
                                         const SpriteDrawing& drawing) {
  const sf::IntRect shownRectangle = rectangleOf(drawing.frame);
  const sf::Vector2f centerPixel =
      sf::Vector2f(drawing.center.x, drawing.center.y) / BACKGROUND_PIXEL_SIZE;
  const sf::Vector2i topLeftPixel =
      nearestPixel(centerPixel - (sf::Vector2f(shownRectangle.size) * HALF));
  sf::Sprite sprite(texture, shownRectangle);
  sprite.setPosition(sf::Vector2f(topLeftPixel));
  return sprite;
}

void SfmlDrawSurface::drawToned(sf::Sprite& sprite, SpriteTone tone) {
  switch (tone) {
    case SpriteTone::Flash:
      drawFlash(sprite);
      return;
    case SpriteTone::Ghost:
      drawGhost(sprite);
      return;
    case SpriteTone::Shade:
      sprite.setColor(SHADE_TINT);
      break;
    case SpriteTone::Normal:
      break;
  }
  canvas_.draw(sprite);
}

void SfmlDrawSurface::drawFlash(sf::Sprite& sprite) {
  if (flashShader_.has_value()) {
    sprite.setColor(FLASH_COLOR);
    const sf::RenderStates states(&*flashShader_);
    canvas_.draw(sprite, states);
    return;
  }
  canvas_.draw(sprite);
  canvas_.draw(sprite, sf::RenderStates(sf::BlendAdd));
}

void SfmlDrawSurface::drawGhost(sf::Sprite& sprite) {
  if (ghostShader_.has_value()) {
    const sf::RenderStates states(&*ghostShader_);
    canvas_.draw(sprite, states);
    return;
  }
  sprite.setColor(GHOST_FALLBACK_TINT);
  canvas_.draw(sprite);
}

void SfmlDrawSurface::drawOutline(const sf::Sprite& sprite, sf::Color color) {
  if (!flashShader_.has_value()) {
    return;
  }
  sf::Sprite silhouette = sprite;
  silhouette.setColor(color);
  const sf::RenderStates states(&*flashShader_);
  for (const sf::Vector2i direction : CROSS_DIRECTIONS) {
    silhouette.setPosition(sprite.getPosition() + sf::Vector2f(direction));
    canvas_.draw(silhouette, states);
  }
}

}  // namespace rtype::client
