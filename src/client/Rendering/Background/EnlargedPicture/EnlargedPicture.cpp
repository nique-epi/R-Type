#include "EnlargedPicture.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include "BackgroundConstants.hpp"
#include "ClientException.hpp"
#include "IntegerScale.hpp"

namespace rtype::client {

sf::BlendMode premultipliedAlphaBlend() {
  return {sf::BlendMode::Factor::One, sf::BlendMode::Factor::OneMinusSrcAlpha};
}

sf::RenderTexture& EnlargedPicture::matching(const sf::RenderTarget& target) {
  const unsigned int factor =
      integerScaleFactor(target.getViewport(target.getView()).size);
  if (factor == factor_) {
    return picture_;
  }
  const sf::Vector2u size = BACKGROUND_SIZE * factor;
  if (!picture_.resize(size)) {
    throw RenderTextureNotCreatedException(size.x, size.y);
  }
  picture_.setSmooth(true);
  factor_ = factor;
  return picture_;
}

unsigned int EnlargedPicture::factor() const { return factor_; }

void EnlargedPicture::layOver(sf::RenderTarget& target,
                              const sf::BlendMode& blend) {
  picture_.display();
  const float remainingScale =
      BACKGROUND_PIXEL_SIZE / static_cast<float>(factor_);
  sf::Sprite picture(picture_.getTexture());
  picture.setScale({remainingScale, remainingScale});
  target.draw(picture, blend);
}

const sf::Texture& EnlargedPicture::texture() const {
  return picture_.getTexture();
}

}  // namespace rtype::client
