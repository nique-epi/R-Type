#include "HighContrastColorsFixtures.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "HighContrastColors.hpp"
#include "HighContrastColorsTestConstants.hpp"

namespace rtype::client::testing {

namespace {

/** @returns The linear value of one sRGB channel, from 0 to 1. */
double linearChannelOf(std::uint8_t channel) {
  const double value = channel / CHANNEL_MAXIMUM;
  return value <= SRGB_LINEAR_LIMIT
             ? value / SRGB_LINEAR_DIVISOR
             : std::pow((value + SRGB_OFFSET) / (1.0 + SRGB_OFFSET),
                        SRGB_EXPONENT);
}

/** @returns The relative luminance of a color, from 0 to 1. */
double luminanceOf(sf::Color color) {
  return (RED_WEIGHT * linearChannelOf(color.r)) +
         (GREEN_WEIGHT * linearChannelOf(color.g)) +
         (BLUE_WEIGHT * linearChannelOf(color.b));
}

}  // namespace

double contrastRatioOf(sf::Color first, sf::Color second) {
  const double lighter = std::max(luminanceOf(first), luminanceOf(second));
  const double darker = std::min(luminanceOf(first), luminanceOf(second));
  return (lighter + LUMINANCE_FLARE) / (darker + LUMINANCE_FLARE);
}

double highContrastRatioOf(TextOnBackground pair) {
  return contrastRatioOf(highContrastOf(pair.text),
                         highContrastOf(pair.background));
}

}  // namespace rtype::client::testing
