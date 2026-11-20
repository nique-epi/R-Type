#include "CasinoLighting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <vector>
#include "BackgroundConstants.hpp"
#include "BeamPair.hpp"
#include "CasinoLightConstants.hpp"
#include "ColoredPixel.hpp"
#include "DurationConversion.hpp"
#include "InterfaceConstants.hpp"
#include "LightBeam.hpp"
#include "RenderingConstants.hpp"
#include "TimeConstants.hpp"
#include "TwinkleBeats.hpp"
#include "TwinkleGroup.hpp"

namespace rtype::client {

namespace {

/** @returns The point @p length away from @p origin at @p angle, in radians
 * from straight up and clockwise. */
sf::Vector2f pointAt(sf::Vector2f origin, float angle, float length) {
  return {origin.x + (std::sin(angle) * length),
          origin.y - (std::cos(angle) * length)};
}

/** @returns On which side of the line from @p from to @p towards @p point
 * lies: its sign alone is read. */
float sideOf(sf::Vector2f point, sf::Vector2f from, sf::Vector2f towards) {
  return ((towards.x - from.x) * (point.y - from.y)) -
         ((towards.y - from.y) * (point.x - from.x));
}

/** @returns Whether @p point lies inside the triangle of @p corners, on
 * whichever side its corners turn. */
bool liesInside(sf::Vector2f point,
                const std::array<sf::Vector2f, 3>& corners) {
  const float first = sideOf(point, corners[0], corners[1]);
  const float second = sideOf(point, corners[1], corners[2]);
  const float third = sideOf(point, corners[2], corners[0]);
  const bool anyBelow = first < 0.0F || second < 0.0F || third < 0.0F;
  const bool anyAbove = first > 0.0F || second > 0.0F || third > 0.0F;
  return !(anyBelow && anyAbove);
}

/** @returns The pixels of the interface the triangle of @p corners may
 * cover: the box around its corners, cut to INTERFACE_AREA. */
sf::IntRect boxAround(const std::array<sf::Vector2f, 3>& corners) {
  const auto [left, right] =
      std::minmax({corners[0].x, corners[1].x, corners[2].x});
  const auto [top, bottom] =
      std::minmax({corners[0].y, corners[1].y, corners[2].y});
  const sf::IntRect box(
      {static_cast<int>(std::floor(left)), static_cast<int>(std::floor(top))},
      {static_cast<int>(std::ceil(right - left)) + 1,
       static_cast<int>(std::ceil(bottom - top)) + 1});
  return box.findIntersection(INTERFACE_AREA).value_or(sf::IntRect{});
}

}  // namespace

bool isLitBulb(sf::Color color) {
  return color.r > LIT_BULB_RED_FLOOR && color.g > LIT_BULB_GREEN_FLOOR &&
         color.b > LIT_BULB_BLUE_FLOOR;
}

float luminanceOf(sf::Color color) {
  return (LUMINANCE_RED_WEIGHT * static_cast<float>(color.r)) +
         (LUMINANCE_GREEN_WEIGHT * static_cast<float>(color.g)) +
         (LUMINANCE_BLUE_WEIGHT * static_cast<float>(color.b));
}

int chaseStepAt(engine::Duration since) {
  return static_cast<int>(since / CASINO_CHASE_STEP);
}

bool isDimmedAt(const TwinkleGroup& group, engine::Duration since) {
  return std::sin((engine::inSeconds(since) * group.speed) + group.phase) >
         group.dimAbove;
}

std::vector<TwinkleGroup> twinkleBeats(const TwinkleBeats& beats) {
  std::vector<TwinkleGroup> groups(CASINO_TWINKLE_GROUPS);
  for (std::size_t beat = 0; beat < groups.size(); ++beat) {
    const float share =
        static_cast<float>(beat) / static_cast<float>(CASINO_TWINKLE_GROUPS);
    groups.at(beat).phase = static_cast<float>(beat) * CASINO_BEAT_SPREAD;
    groups.at(beat).speed = beats.slowest + (share * beats.speedRange);
    groups.at(beat).dimAbove = beats.dimAbove;
  }
  return groups;
}

sf::Color withShareOfLight(sf::Color color, float share) {
  sf::Color shaded = color;
  shaded.a = static_cast<std::uint8_t>(
      std::clamp(share, 0.0F, 1.0F) *
      static_cast<float>(std::numeric_limits<std::uint8_t>::max()));
  return shaded;
}

sf::Color pixelColorAt(const sf::Image& picture, sf::Vector2i pixel) {
  return picture.getPixel(sf::Vector2u(pixel));
}

bool liesInAny(sf::Vector2i pixel, std::span<const sf::IntRect> areas) {
  return std::ranges::any_of(areas, [pixel](const sf::IntRect& area) noexcept {
    return area.contains(pixel);
  });
}

std::uint32_t scrambledAt(sf::Vector2i pixel) {
  return scrambledOf(scrambledOf(static_cast<std::uint32_t>(pixel.x)) ^
                     static_cast<std::uint32_t>(pixel.y));
}

std::uint32_t scrambledOf(std::uint32_t value) {
  std::uint32_t scrambled = value;
  scrambled ^= scrambled >> SCRAMBLE_FIRST_SHIFT;
  scrambled *= SCRAMBLE_FIRST_FACTOR;
  scrambled ^= scrambled >> SCRAMBLE_SECOND_SHIFT;
  scrambled *= SCRAMBLE_SECOND_FACTOR;
  scrambled ^= scrambled >> SCRAMBLE_FIRST_SHIFT;
  return scrambled;
}

void addBeamPixels(std::vector<ColoredPixel>& pixels, const LightBeam& beam) {
  const std::array<sf::Vector2f, 3> corners{
      beam.origin,
      pointAt(beam.origin, beam.angle - beam.halfSpread, beam.length),
      pointAt(beam.origin, beam.angle + beam.halfSpread, beam.length)};
  const sf::IntRect box = boxAround(corners);
  const sf::Vector2f middle{HALF, HALF};
  for (int row = box.position.y; row < box.position.y + box.size.y; ++row) {
    for (int column = box.position.x; column < box.position.x + box.size.x;
         ++column) {
      if (liesInside(sf::Vector2f(sf::Vector2i(column, row)) + middle,
                     corners)) {
        pixels.push_back(
            ColoredPixel{.position = {column, row}, .color = beam.color});
      }
    }
  }
}

void swingBeams(std::vector<ColoredPixel>& pixels, const BeamPair& pair,
                engine::Duration since) {
  const float swing =
      std::sin(engine::inSeconds(since) * FULL_TURN / pair.period) * pair.swing;
  LightBeam left = pair.left;
  left.angle += swing;
  LightBeam right = pair.right;
  right.angle -= swing;
  pixels.clear();
  addBeamPixels(pixels, left);
  addBeamPixels(pixels, right);
}

}  // namespace rtype::client
