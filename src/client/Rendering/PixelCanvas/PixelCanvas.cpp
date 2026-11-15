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
#include "EnlargedPicture.hpp"
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

sf::View PixelCanvas::pixelView() {
  return sf::View(sf::FloatRect({0.0F, 0.0F}, sf::Vector2f(BACKGROUND_SIZE)));
}

void PixelCanvas::show(sf::RenderTarget& target) const {
  sf::RenderTexture& enlarged = enlargedPicture_.matching(target);
  sf::Sprite sharpPicture(canvas_.getTexture());
  const float sharpScale = sharpScaleOf(enlargedPicture_.factor(), density_);
  sharpPicture.setScale({sharpScale, sharpScale});
  enlarged.draw(sharpPicture, sf::BlendNone);
  enlargedPicture_.layOver(target, premultipliedAlphaBlend());
}

}  // namespace rtype::client
