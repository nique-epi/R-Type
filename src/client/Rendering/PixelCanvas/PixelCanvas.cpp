#include "PixelCanvas.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "BackgroundConstants.hpp"
#include "ClientException.hpp"
#include "EnlargedPicture.hpp"

namespace rtype::client {

PixelCanvas::PixelCanvas() {
  if (!canvas_.resize(BACKGROUND_SIZE)) {
    throw RenderTextureNotCreatedException(BACKGROUND_SIZE.x,
                                           BACKGROUND_SIZE.y);
  }
  canvas_.clear(sf::Color::Transparent);
  canvas_.display();
}

sf::RenderTexture& PixelCanvas::canvas() { return canvas_; }

void PixelCanvas::show(sf::RenderTarget& target) const {
  sf::RenderTexture& enlarged = enlargedPicture_.matching(target);
  const auto factor = static_cast<float>(enlargedPicture_.factor());
  sf::Sprite sharpPicture(canvas_.getTexture());
  sharpPicture.setScale({factor, factor});
  enlarged.draw(sharpPicture, sf::BlendNone);
  enlargedPicture_.layOver(target, premultipliedAlphaBlend());
}

}  // namespace rtype::client
