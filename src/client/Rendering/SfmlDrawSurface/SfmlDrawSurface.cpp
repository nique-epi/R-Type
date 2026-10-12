#include "SfmlDrawSurface.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <string_view>
#include "BackgroundConstants.hpp"
#include "ITextureSource.hpp"
#include "PixelRounding.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

SfmlDrawSurface::SfmlDrawSurface(sf::RenderTarget& canvas,
                                 const ITextureSource& textures)
    : canvas_(canvas),
      textures_(textures),
      logger_(std::string(LOG_MODULE_NAME)) {}

void SfmlDrawSurface::draw(std::string_view assetId, game::SpriteFrame frame,
                           game::Position center) {
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
  canvas_.draw(sprite);
}

}  // namespace rtype::client
