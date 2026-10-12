#include "ScrollingBackground.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cstdint>
#include "BackgroundColors.hpp"
#include "BackgroundConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ScrollingBackground::ScrollingBackground(std::uint32_t seed)
    : starfield_(seed) {}

void ScrollingBackground::advance(engine::Duration elapsed) {
  starfield_.advance(elapsed);
}

void ScrollingBackground::draw(sf::RenderTarget& target) {
  pixels_.clear();
  starfield_.paint(pixels_);
  sf::RenderTexture& sky = sky_.canvas();
  sky.clear(SKY_COLOR);
  sky.draw(pixels_);
  sky.display();
  sky_.show(target);
  if (!halo_.isAvailable()) {
    return;
  }
  sf::Sprite halo(halo_.apply(sky.getTexture()));
  halo.setScale({BACKGROUND_PIXEL_SIZE, BACKGROUND_PIXEL_SIZE});
  halo.setColor(HALO_TINT);
  target.draw(halo, sf::BlendAdd);
}

}  // namespace rtype::client
