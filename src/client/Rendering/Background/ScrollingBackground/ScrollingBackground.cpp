#include "ScrollingBackground.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "ShiftedView.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ScrollingBackground::ScrollingBackground(std::uint32_t seed)
    : starfield_(seed) {}

void ScrollingBackground::advance(engine::Duration elapsed) {
  starfield_.advance(elapsed);
}

void ScrollingBackground::setSpeedFactor(float factor) {
  starfield_.setSpeedFactor(factor);
}

void ScrollingBackground::setOffset(sf::Vector2i pixels) { offset_ = pixels; }

void ScrollingBackground::draw(sf::RenderTarget& target) {
  pixels_.clear();
  starfield_.paint(pixels_);
  sf::RenderTexture& sky = sky_.canvas();
  sky.clear(SKY_COLOR);
  sky.setView(shiftedBy(sky.getDefaultView(), offset_));
  sky.draw(pixels_);
  sky.setView(sky.getDefaultView());
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
