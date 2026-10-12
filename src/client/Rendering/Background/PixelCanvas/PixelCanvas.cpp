#include "PixelCanvas.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include "BackgroundConstants.hpp"
#include "ClientException.hpp"
#include "IntegerScale.hpp"

namespace rtype::client {

PixelCanvas::PixelCanvas() {
  if (!canvas_.resize(BACKGROUND_SIZE)) {
    throw RenderTextureNotCreatedException(BACKGROUND_SIZE.x,
                                           BACKGROUND_SIZE.y);
  }
}

sf::RenderTexture& PixelCanvas::canvas() { return canvas_; }

void PixelCanvas::show(sf::RenderTarget& target) {
  const unsigned int factor =
      integerScaleFactor(target.getViewport(target.getView()).size);
  matchEnlargedPicture(factor);
  sf::Sprite sharpPicture(canvas_.getTexture());
  sharpPicture.setScale(
      {static_cast<float>(factor), static_cast<float>(factor)});
  enlargedPicture_.draw(sharpPicture, sf::BlendNone);
  enlargedPicture_.display();

  const float remainingScale =
      BACKGROUND_PIXEL_SIZE / static_cast<float>(factor);
  sf::Sprite picture(enlargedPicture_.getTexture());
  picture.setScale({remainingScale, remainingScale});
  const sf::BlendMode premultipliedAlpha(
      sf::BlendMode::Factor::One, sf::BlendMode::Factor::OneMinusSrcAlpha);
  target.draw(picture, premultipliedAlpha);
}

void PixelCanvas::matchEnlargedPicture(unsigned int factor) {
  if (factor == enlargedPictureFactor_) {
    return;
  }
  const sf::Vector2u size = BACKGROUND_SIZE * factor;
  if (!enlargedPicture_.resize(size)) {
    throw RenderTextureNotCreatedException(size.x, size.y);
  }
  enlargedPicture_.setSmooth(true);
  enlargedPictureFactor_ = factor;
}

}  // namespace rtype::client
