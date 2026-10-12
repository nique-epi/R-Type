#include "Comet.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <random>
#include "BackgroundColors.hpp"
#include "BackgroundConstants.hpp"
#include "IPixelSurface.hpp"
#include "PixelRounding.hpp"
#include "RandomRange.hpp"
#include "SkyEventConstants.hpp"
#include "TailGradient.hpp"

namespace rtype::client {

Comet::Comet(std::mt19937& random)
    : head_(BACKGROUND_WIDTH + COMET_ENTRY_MARGIN,
            randomBetween(random, COMET_EDGE_MARGIN,
                          BACKGROUND_HEIGHT - COMET_EDGE_MARGIN)),
      velocity_(randomPointBetween(
          random, {-COMET_FASTEST_SPEED, -COMET_LARGEST_DRIFT},
          {-COMET_SLOWEST_SPEED, COMET_LARGEST_DRIFT})),
      direction_(velocity_.normalized()),
      flickerRandom_(random()) {}

void Comet::advance(float eventSeconds) {
  head_ += velocity_ * eventSeconds;
  gapPhase_ += COMET_GAP_STEPS_PER_SECOND * eventSeconds;
  for (std::size_t i = 1; i < COMET_TAIL_LENGTH; ++i) {
    flickeredOut_[i] = tailFraction(i) > COMET_FLICKER_START &&
                       randomChance(flickerRandom_, COMET_FLICKER_CHANCE);
  }
}

bool Comet::isOver() const { return head_.x <= -COMET_OFF_SKY_MARGIN; }

void Comet::paint(IPixelSurface& surface) const {
  const auto gapOffset = static_cast<std::size_t>(gapPhase_);
  for (std::size_t i = 1; i < COMET_TAIL_LENGTH; ++i) {
    const bool inGap =
        i > COMET_SOLID_TAIL && (i + gapOffset) % COMET_GAP_PERIOD == 0;
    if (inGap || flickeredOut_.test(i)) {
      continue;
    }
    const sf::Color color = tailColor(COMET_GRADIENT, tailFraction(i));
    const sf::Vector2i pixel =
        nearestPixel(head_ - (direction_ * static_cast<float>(i)));
    surface.plot(pixel, color);
    if (i < COMET_THICK_TAIL && i % COMET_THICK_TAIL_STRIDE == 1) {
      surface.plot(pixel + PIXEL_ABOVE, color);
      surface.plot(pixel + PIXEL_BELOW, color);
    }
  }
  const sf::Vector2i head = nearestPixel(head_);
  for (const sf::Vector2i offset : COMET_HEAD_OFFSETS) {
    surface.plot(head + offset, STAR_WHITE);
  }
}

float Comet::tailFraction(std::size_t pixel) {
  return static_cast<float>(pixel) / static_cast<float>(COMET_TAIL_LENGTH);
}

}  // namespace rtype::client
