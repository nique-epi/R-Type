#include "Flare.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <numbers>
#include <random>
#include "BackgroundColors.hpp"
#include "BackgroundConstants.hpp"
#include "IPixelSurface.hpp"
#include "RandomRange.hpp"
#include "SkyEventConstants.hpp"

namespace rtype::client {

Flare::Flare(std::mt19937& random)
    : center_(randomPointBetween(random, {FLARE_EDGE_MARGIN, FLARE_EDGE_MARGIN},
                                 {BACKGROUND_WIDTH - FLARE_EDGE_MARGIN,
                                  BACKGROUND_HEIGHT - FLARE_EDGE_MARGIN})),
      duration_(randomBetween(random, FLARE_SHORTEST_DURATION,
                              FLARE_LONGEST_DURATION)) {}

void Flare::advance(float eventSeconds) { age_ += eventSeconds; }

bool Flare::isOver() const { return age_ >= duration_; }

void Flare::paint(IPixelSurface& surface) const {
  const float glow = std::sin(age_ / duration_ * std::numbers::pi_v<float>);
  const int armLength = static_cast<int>(std::lround(glow * FLARE_LONGEST_ARM));
  for (int i = 1; i <= armLength; ++i) {
    const sf::Color color = i == armLength ? BLUE : PALE_BLUE;
    for (const sf::Vector2i direction : CROSS_DIRECTIONS) {
      surface.plot(center_ + (direction * i), color);
    }
  }
  if (armLength >= FLARE_SHORTEST_ARM_WITH_DIAGONALS) {
    for (const sf::Vector2i direction : DIAGONAL_DIRECTIONS) {
      surface.plot(center_ + direction, BLUE);
    }
  }
  surface.plot(center_, glow > FLARE_WHITE_CORE_GLOW ? STAR_WHITE : STAR_GOLD);
}

}  // namespace rtype::client
