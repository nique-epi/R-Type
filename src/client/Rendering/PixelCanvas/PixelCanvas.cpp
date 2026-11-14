#include "PixelCanvas.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include "BackgroundConstants.hpp"
#include "ClientException.hpp"
#include "IntegerScale.hpp"

namespace rtype::client {

PixelCanvas::PixelCanvas(unsigned int density) : density_(density) {
  const sf::Vector2u size = BACKGROUND_SIZE * density_;
  if (!canvas_.resize(size)) {
    throw RenderTextureNotCreatedException(size.x, size.y);
  }
  canvas_.setView(pixelView());
  canvas_.clear(sf::Color::Transparent);
  canvas_.display();
}

sf::RenderTexture& PixelCanvas::canvas() { return canvas_; }

sf::View PixelCanvas::pixelView() const {
  return sf::View(sf::FloatRect({0.0F, 0.0F}, sf::Vector2f(BACKGROUND_SIZE)));
}

void PixelCanvas::show(sf::RenderTarget& target) const {
  const unsigned int factor =
      integerScaleFactor(target.getViewport(target.getView()).size);
  matchEnlargedPicture(factor);
  sf::Sprite sharpPicture(canvas_.getTexture());
  const float sharpScale = sharpScaleOf(factor, density_);
  sharpPicture.setScale({sharpScale, sharpScale});
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

void PixelCanvas::matchEnlargedPicture(unsigned int factor) const {
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
