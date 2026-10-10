#include "ScrollingBackground.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "BackgroundColors.hpp"
#include "BackgroundConstants.hpp"
#include "ClientException.hpp"
#include "IntegerScale.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ScrollingBackground::ScrollingBackground(std::uint32_t seed)
    : starfield_(seed) {
  if (!sky_.resize(BACKGROUND_SIZE)) {
    throw RenderTextureNotCreatedException(BACKGROUND_SIZE.x,
                                           BACKGROUND_SIZE.y);
  }
}

void ScrollingBackground::advance(engine::Duration elapsed) {
  starfield_.advance(elapsed);
}

void ScrollingBackground::draw(sf::RenderTarget& target) {
  pixels_.clear();
  starfield_.paint(pixels_);
  sky_.clear(SKY_COLOR);
  sky_.draw(pixels_);
  sky_.display();

  const unsigned int factor =
      integerScaleFactor(target.getViewport(target.getView()).size);
  matchEnlargedSky(factor);
  sf::Sprite sharpSky(sky_.getTexture());
  sharpSky.setScale({static_cast<float>(factor), static_cast<float>(factor)});
  enlargedSky_.clear();
  enlargedSky_.draw(sharpSky);
  enlargedSky_.display();

  const float remainingScale =
      BACKGROUND_PIXEL_SIZE / static_cast<float>(factor);
  sf::Sprite sky(enlargedSky_.getTexture());
  sky.setScale({remainingScale, remainingScale});
  target.draw(sky);
  if (!halo_.isAvailable()) {
    return;
  }
  sf::Sprite halo(halo_.apply(sky_.getTexture()));
  halo.setScale({BACKGROUND_PIXEL_SIZE, BACKGROUND_PIXEL_SIZE});
  halo.setColor(HALO_TINT);
  target.draw(halo, sf::BlendAdd);
}

void ScrollingBackground::matchEnlargedSky(unsigned int factor) {
  if (factor == enlargedSkyFactor_) {
    return;
  }
  const sf::Vector2u size = BACKGROUND_SIZE * factor;
  if (!enlargedSky_.resize(size)) {
    throw RenderTextureNotCreatedException(size.x, size.y);
  }
  enlargedSky_.setSmooth(true);
  enlargedSkyFactor_ = factor;
}

}  // namespace rtype::client
