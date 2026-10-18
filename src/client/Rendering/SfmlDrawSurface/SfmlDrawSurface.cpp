#include "SfmlDrawSurface.hpp"
#include <SFML/Graphics/BlendMode.hpp>
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
#include "FlashShader.hpp"
#include "FragmentShader.hpp"
#include "ITextureSource.hpp"
#include "PixelRounding.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "SpriteToneColors.hpp"

namespace rtype::client {

SfmlDrawSurface::SfmlDrawSurface(sf::RenderTarget& canvas,
                                 const ITextureSource& textures)
    : canvas_(canvas),
      textures_(textures),
      logger_(std::string(LOG_MODULE_NAME)),
      flashShader_(loadFragmentShader(FLASH_SHADER, logger_)) {}

void SfmlDrawSurface::draw(std::string_view assetId, game::SpriteFrame frame,
                           game::Position center, SpriteTone tone) {
  const sf::Texture* texture = textures_.texture(assetId);
  if (texture == nullptr) {
    if (reportedMissingIds_.emplace(assetId).second) {
      logger_.warn("no texture for asset id ", assetId);
    }
    return;
  }
  const sf::IntRect shownRectangle({frame.left, frame.top},
                                   {frame.width, frame.height});
  const sf::Vector2f centerPixel =
      sf::Vector2f(center.x, center.y) / BACKGROUND_PIXEL_SIZE;
  const sf::Vector2i topLeftPixel =
      nearestPixel(centerPixel - (sf::Vector2f(shownRectangle.size) * HALF));
  sf::Sprite sprite(*texture, shownRectangle);
  sprite.setPosition(sf::Vector2f(topLeftPixel));
  switch (tone) {
    case SpriteTone::Flash:
      drawFlash(sprite);
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

}  // namespace rtype::client
