#include "ShootingStar.hpp"
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>
#include "BackgroundConstants.hpp"
#include "IPixelSurface.hpp"
#include "PixelRounding.hpp"
#include "RandomRange.hpp"
#include "SkyEventConstants.hpp"
#include "TailGradient.hpp"

namespace rtype::client {

ShootingStar::ShootingStar(std::mt19937& random)
    : speed_(randomBetween(random, SHOOTING_STAR_SLOWEST_SPEED,
                           SHOOTING_STAR_FASTEST_SPEED)),
      direction_(1.0F, sf::radians(
                           std::numbers::pi_v<float> -
                           randomBetween(random, SHOOTING_STAR_SHALLOWEST_ANGLE,
                                         SHOOTING_STAR_STEEPEST_ANGLE))),
      head_(randomPointBetween(
          random, {SHOOTING_STAR_LEFTMOST_START * BACKGROUND_WIDTH, 0.0F},
          {BACKGROUND_WIDTH, SHOOTING_STAR_LOWEST_START * BACKGROUND_HEIGHT})),
      remainingLife_(randomBetween(random, SHOOTING_STAR_SHORTEST_LIFE,
                                   SHOOTING_STAR_LONGEST_LIFE)) {}

void ShootingStar::advance(float eventSeconds) {
  head_ += direction_ * (speed_ * eventSeconds);
  remainingLife_ -= eventSeconds;
  if (remainingLife_ > 0.0F) {
    trailLength_ =
        std::min(SHOOTING_STAR_LONGEST_TRAIL,
                 trailLength_ + (SHOOTING_STAR_TRAIL_GROWTH * eventSeconds));
  } else {
    trailLength_ -= SHOOTING_STAR_TRAIL_SHRINK * eventSeconds;
  }
}

bool ShootingStar::isOver() const {
  return trailLength_ <= 0.0F && remainingLife_ <= 0.0F;
}

void ShootingStar::paint(IPixelSurface& surface) const {
  const int pixelCount =
      std::max(1, static_cast<int>(std::lround(trailLength_)));
  for (int i = 0; i < pixelCount; ++i) {
    const float fraction =
        static_cast<float>(i) / static_cast<float>(pixelCount);
    surface.plot(nearestPixel(head_ - (direction_ * static_cast<float>(i))),
                 tailColor(SHOOTING_STAR_GRADIENT, fraction));
  }
}

}  // namespace rtype::client
