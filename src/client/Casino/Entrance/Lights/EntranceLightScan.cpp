#include "EntranceLightScan.hpp"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <optional>
#include <span>
#include <vector>
#include "BackgroundConstants.hpp"
#include "CasinoLighting.hpp"
#include "ColoredPixel.hpp"
#include "EntranceLightConstants.hpp"
#include "EntranceLights.hpp"
#include "InterfaceConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The rank of @p pixel along the oval of bulbs, by its angle around
 * the middle of the oval; nothing when it lies off the band of the oval. */
std::optional<int> ovalRankOf(sf::Vector2i pixel) {
  const float across = (static_cast<float>(pixel.x) - ENTRANCE_OVAL_MIDDLE.x) /
                       ENTRANCE_OVAL_RADII.x;
  const float down = (static_cast<float>(pixel.y) - ENTRANCE_OVAL_MIDDLE.y) /
                     ENTRANCE_OVAL_RADII.y;
  const float distance = std::hypot(across, down);
  if (distance < ENTRANCE_OVAL_INNER || distance > ENTRANCE_OVAL_OUTER ||
      pixel.y < ENTRANCE_OVAL_TOP || pixel.y >= ENTRANCE_OVAL_BOTTOM) {
    return std::nullopt;
  }
  const float turn =
      (std::atan2(down, across) + std::numbers::pi_v<float>) / FULL_TURN;
  return static_cast<int>(turn * static_cast<float>(ENTRANCE_OVAL_RANKS));
}

/** @brief Files @p pixel of the golden discs, which is not a bulb, on its
 * row, both lit and shaded. */
void fileDisc(EntranceLights& lights, sf::Vector2i pixel) {
  const auto row = static_cast<std::size_t>(
      (pixel.y - ENTRANCE_DISCS_AREA.position.y) / ENTRANCE_DISC_ROW_HEIGHT);
  if (lights.litDiscRows.size() <= row) {
    lights.litDiscRows.resize(row + 1);
    lights.shadedDiscRows.resize(row + 1);
  }
  lights.litDiscRows.at(row).push_back(
      ColoredPixel{.position = pixel, .color = ENTRANCE_DISC_GLOW});
  lights.shadedDiscRows.at(row).push_back(
      ColoredPixel{.position = pixel, .color = ENTRANCE_DISC_SHADE});
}

/** @brief Files the lit bulb @p pixel of the pylon, off its discs: a
 * diamond, a column, the script, or the oval. */
void filePylonBulb(EntranceLights& lights, sf::Vector2i pixel) {
  const ColoredPixel diamond{.position = pixel,
                             .color = ENTRANCE_UNLIT_DIAMOND};
  if (liesInAny(pixel, ENTRANCE_LEFT_DIAMONDS)) {
    lights.diamonds[0].push_back(diamond);
  } else if (liesInAny(pixel, ENTRANCE_RIGHT_DIAMONDS)) {
    lights.diamonds[1].push_back(diamond);
  } else if (liesInAny(pixel, ENTRANCE_COLUMNS)) {
    const int bottom =
        ENTRANCE_PYLON_AREA.position.y + ENTRANCE_PYLON_AREA.size.y;
    lights.columns.add(pixel, (bottom - pixel.y) / ENTRANCE_COLUMN_RANK);
  } else if (ENTRANCE_NUMERALS_AREA.contains(pixel)) {
    return;
  } else if (ENTRANCE_SCRIPT_AREA.contains(pixel)) {
    lights.darkScript.push_back(
        ColoredPixel{.position = pixel, .color = ENTRANCE_DARK_NEON});
  } else if (const std::optional<int> rank = ovalRankOf(pixel)) {
    lights.oval.add(pixel, *rank);
  }
}

/** @brief Reads the pylon of @p picture into @p lights. */
void readPylon(EntranceLights& lights, const sf::Image& picture) {
  const sf::IntRect pylon = ENTRANCE_PYLON_AREA;
  for (int pixelRow = pylon.position.y;
       pixelRow < pylon.position.y + pylon.size.y; ++pixelRow) {
    for (int pixelColumn = pylon.position.x;
         pixelColumn < pylon.position.x + pylon.size.x; ++pixelColumn) {
      const bool lit =
          isLitBulb(pixelColorAt(picture, {pixelColumn, pixelRow}));
      if (ENTRANCE_DISCS_AREA.contains({pixelColumn, pixelRow})) {
        if (!lit) {
          fileDisc(lights, {pixelColumn, pixelRow});
        }
      } else if (lit) {
        filePylonBulb(lights, {pixelColumn, pixelRow});
      }
    }
  }
}

/** @returns Whether @p pixel of @p picture is a star or a lit window: bright,
 * and brighter than its four neighbours by ENTRANCE_STAR_CONTRAST. */
bool isStar(const sf::Image& picture, sf::Vector2i pixel) {
  const float brightness = luminanceOf(pixelColorAt(picture, pixel));
  const float around =
      (luminanceOf(pixelColorAt(picture, {pixel.x - 1, pixel.y})) +
       luminanceOf(pixelColorAt(picture, {pixel.x + 1, pixel.y})) +
       luminanceOf(pixelColorAt(picture, {pixel.x, pixel.y - 1})) +
       luminanceOf(pixelColorAt(picture, {pixel.x, pixel.y + 1}))) /
      static_cast<float>(ENTRANCE_STAR_NEIGHBOURS);
  return brightness > ENTRANCE_STAR_LUMINANCE &&
         brightness - around > ENTRANCE_STAR_CONTRAST;
}

/** @brief Files @p pixel of the sky as a star when it is one and twinkles:
 * one star in ENTRANCE_TWINKLING_SHARE, on the beat it scrambles to. */
void fileStar(EntranceLights& lights, const sf::Image& picture,
              sf::Vector2i pixel) {
  if (pixel.y >= ENTRANCE_SKY_BOTTOM || ENTRANCE_PYLON_AREA.contains(pixel) ||
      liesInAny(pixel, ENTRANCE_ROOF_SILHOUETTES) || !isStar(picture, pixel)) {
    return;
  }
  const std::uint32_t draw = scrambledAt(pixel);
  if (draw % ENTRANCE_TWINKLING_SHARE != 0) {
    return;
  }
  lights.stars.at((draw / ENTRANCE_TWINKLING_SHARE) % lights.stars.size())
      .pixels.push_back(
          ColoredPixel{.position = pixel, .color = ENTRANCE_DIM_STAR});
}

/** @brief Reads the marquee and the sky of @p picture into @p lights, every
 * pixel but those of its edges, whose neighbours a star compares with. */
void readMarqueeAndSky(EntranceLights& lights, const sf::Image& picture) {
  const sf::Vector2i size(picture.getSize());
  for (int pixelRow = 1; pixelRow + 1 < size.y; ++pixelRow) {
    for (int pixelColumn = 1; pixelColumn + 1 < size.x; ++pixelColumn) {
      if (ENTRANCE_MARQUEE_AREA.contains({pixelColumn, pixelRow}) &&
          !ENTRANCE_MARQUEE_LETTERS.contains({pixelColumn, pixelRow})) {
        if (isLitBulb(pixelColorAt(picture, {pixelColumn, pixelRow}))) {
          lights.marquee.add({pixelColumn, pixelRow},
                             pixelColumn / ENTRANCE_MARQUEE_RANK_WIDTH);
        }
      } else {
        fileStar(lights, picture, {pixelColumn, pixelRow});
      }
    }
  }
}

}  // namespace

EntranceLights entranceLightsOf(const sf::Image& picture) {
  if (sf::Vector2i(picture.getSize()) != INTERFACE_AREA.size) {
    return EntranceLights{};
  }
  EntranceLights lights;
  lights.stars = twinkleBeats(ENTRANCE_STAR_BEATS);
  readPylon(lights, picture);
  readMarqueeAndSky(lights, picture);
  return lights;
}

}  // namespace rtype::client
