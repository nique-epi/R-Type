#include "Asteroid.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <random>
#include "BackgroundColors.hpp"
#include "BackgroundConstants.hpp"
#include "IPixelSurface.hpp"
#include "PixelRounding.hpp"
#include "RandomRange.hpp"
#include "SkyEventConstants.hpp"

namespace rtype::client {

Asteroid::Asteroid(std::mt19937& random)
    : topLeft_(BACKGROUND_WIDTH + ASTEROID_OFF_SKY_MARGIN,
               randomBetween(random, 0.0F, BACKGROUND_HEIGHT)),
      velocity_(randomPointBetween(
          random, {-ASTEROID_FASTEST_SPEED, -ASTEROID_LARGEST_DRIFT},
          {-ASTEROID_SLOWEST_SPEED, ASTEROID_LARGEST_DRIFT})),
      spinSpeed_(
          randomBetween(random, ASTEROID_SLOWEST_SPIN, ASTEROID_FASTEST_SPIN)),
      frame_(randomBetween(random, 0.0F,
                           static_cast<float>(ASTEROID_FRAMES.size()))) {}

void Asteroid::advance(float eventSeconds) {
  topLeft_ += velocity_ * eventSeconds;
  frame_ += spinSpeed_ * eventSeconds;
}

bool Asteroid::isOver() const { return topLeft_.x <= -ASTEROID_OFF_SKY_MARGIN; }

void Asteroid::paint(IPixelSurface& surface) const {
  const AsteroidFrame& rows = ASTEROID_FRAMES[static_cast<std::size_t>(frame_) %
                                              ASTEROID_FRAMES.size()];
  const sf::Vector2i topLeft = nearestPixel(topLeft_);
  for (std::size_t row = 0; row < rows.size(); ++row) {
    for (std::size_t column = 0; column < rows[row].size(); ++column) {
      const char shade = rows[row][column];
      if (shade == ASTEROID_EMPTY) {
        continue;
      }
      surface.plot(topLeft + sf::Vector2i(static_cast<int>(column),
                                          static_cast<int>(row)),
                   shade == ASTEROID_LIGHT ? ASTEROID_LIGHT_COLOR
                                           : ASTEROID_SHADOW_COLOR);
    }
  }
}

}  // namespace rtype::client
