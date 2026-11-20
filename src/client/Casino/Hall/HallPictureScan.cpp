#include "HallPictureScan.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "CasinoHallConstants.hpp"
#include "CasinoLighting.hpp"
#include "ColoredPixel.hpp"
#include "HallPicture.hpp"
#include "InterfaceConstants.hpp"
#include "Spectator.hpp"
#include "SpectatorColumns.hpp"
#include "TwinkleGroup.hpp"

namespace rtype::client {

namespace {

/** @brief The pixels of one band of rows of a picture that belong to the
 * crowd, row by row from its top. */
using CrowdRows = std::vector<std::vector<bool>>;

/** @returns The color of the pixel of @p picture at @p pixel. */
sf::Color colorAt(const sf::Image& picture, sf::Vector2i pixel) {
  return picture.getPixel(sf::Vector2u(pixel));
}

/** @returns Whether @p pixel lies in one of @p areas. */
bool liesInAny(sf::Vector2i pixel, std::span<const sf::IntRect> areas) {
  return std::ranges::any_of(
      areas, [pixel](const sf::IntRect& area) { return area.contains(pixel); });
}

/** @returns Whether @p color is a lit bulb or crystal of the hall, brighter
 * than the bulbs of the front of the casino. */
bool isHallBulb(sf::Color color) {
  return color.r > HALL_BULB_RED_FLOOR && color.g > HALL_BULB_GREEN_FLOOR &&
         color.b > HALL_BULB_BLUE_FLOOR;
}

/** @returns The HALL_TWINKLE_GROUPS beats the crystals are spread over, each
 * with a phase and a speed of its own, and no crystal yet. */
std::vector<TwinkleGroup> crystalBeats() {
  std::vector<TwinkleGroup> beats(HALL_TWINKLE_GROUPS);
  for (std::size_t beat = 0; beat < beats.size(); ++beat) {
    const float share =
        static_cast<float>(beat) / static_cast<float>(HALL_TWINKLE_GROUPS);
    beats.at(beat).phase = static_cast<float>(beat) * HALL_BEAT_SPREAD;
    beats.at(beat).speed =
        HALL_SLOWEST_CRYSTAL + (share * HALL_CRYSTAL_SPEED_RANGE);
    beats.at(beat).dimAbove = HALL_CRYSTAL_DIM_ABOVE;
  }
  return beats;
}

/** @brief Files the lit pixel @p pixel of the hall: a crystal of a
 * chandelier on a beat it scrambles to, or a bulb of the chase. */
void fileLight(HallPicture& hall, sf::Vector2i pixel) {
  if (liesInAny(pixel, HALL_CHANDELIERS)) {
    const std::uint32_t beat =
        scrambledOf(scrambledOf(static_cast<std::uint32_t>(pixel.x)) ^
                    static_cast<std::uint32_t>(pixel.y)) %
        HALL_TWINKLE_GROUPS;
    hall.crystals.at(beat).pixels.push_back(
        ColoredPixel{.position = pixel, .color = HALL_DIM_CRYSTAL});
  } else if (liesInAny(pixel, HALL_BULB_AREAS)) {
    hall.bulbs.add(pixel, (pixel.x + pixel.y) / HALL_BULB_RANK_WIDTH);
  }
}

/** @returns Whether @p color is rope: bright, or red. */
bool isRope(sf::Color color) {
  return luminanceOf(color) > HALL_ROPE_LUMINANCE_FLOOR ||
         (color.r > HALL_RED_ROPE_RED_FLOOR &&
          color.g < HALL_RED_ROPE_GREEN_CEILING);
}

/** @returns The pixels of the front ropes of the ring in @p picture. */
std::vector<ColoredPixel> frontRopesOf(const sf::Image& picture) {
  std::vector<ColoredPixel> ropes;
  for (const int row : HALL_FRONT_ROPE_ROWS) {
    for (int x = HALL_FRONT_ROPES_LEFT; x < HALL_FRONT_ROPES_RIGHT; ++x) {
      const sf::Color color = colorAt(picture, {x, row});
      if (isRope(color)) {
        ropes.push_back(ColoredPixel{.position = {x, row}, .color = color});
      }
    }
  }
  return ropes;
}

/** @returns Whether @p color is the red carpet under the crowd. */
bool isCarpet(sf::Color color) {
  const auto red = static_cast<float>(color.r);
  return red > static_cast<float>(color.b) * HALL_CARPET_RED_OVER_BLUE &&
         static_cast<float>(color.g) < red * HALL_CARPET_GREEN_OVER_RED;
}

/** @returns Whether @p color is the dark mass of the crowd. */
bool isCrowdMass(sf::Color color) {
  return luminanceOf(color) < HALL_CROWD_LUMINANCE_CEILING &&
         !(color.r > HALL_CARPET_RED_FLOOR && isCarpet(color));
}

/** @returns The dark mass of the crowd of @p picture, row by row from
 * HALL_CROWD_TOP. */
CrowdRows crowdMassOf(const sf::Image& picture) {
  const int width = static_cast<int>(picture.getSize().x);
  CrowdRows mass(HALL_CROWD_BOTTOM - HALL_CROWD_TOP,
                 std::vector<bool>(static_cast<std::size_t>(width)));
  for (std::size_t row = 0; row < mass.size(); ++row) {
    for (int x = 0; x < width; ++x) {
      mass[row][static_cast<std::size_t>(x)] = isCrowdMass(
          colorAt(picture, {x, HALL_CROWD_TOP + static_cast<int>(row)}));
    }
  }
  return mass;
}

/** @returns Whether the pixel at @p column of @p row touches the mass of
 * @p mass on one of its four sides. */
bool touchesTheMass(const CrowdRows& mass, std::size_t row,
                    std::size_t column) {
  const std::vector<bool>& line = mass[row];
  return (column > 0 && line[column - 1]) ||
         (column + 1 < line.size() && line[column + 1]) ||
         (row > 0 && mass[row - 1][column]) ||
         (row + 1 < mass.size() && mass[row + 1][column]);
}

/** @returns The crowd of @p picture: its dark mass and the rim lights that
 * touch it, but the carpet. */
CrowdRows crowdOf(const sf::Image& picture) {
  const CrowdRows mass = crowdMassOf(picture);
  CrowdRows crowd = mass;
  for (std::size_t row = 0; row < mass.size(); ++row) {
    for (std::size_t column = 1; column + 1 < mass[row].size(); ++column) {
      if (!mass[row][column] && touchesTheMass(mass, row, column) &&
          !isCarpet(
              colorAt(picture, {static_cast<int>(column),
                                HALL_CROWD_TOP + static_cast<int>(row)}))) {
        crowd[row][column] = true;
      }
    }
  }
  return crowd;
}

/** @returns For each column of @p crowd, the row of its highest pixel of
 * crowd, or the height of the band when the column has none. */
std::vector<int> skylineOf(const CrowdRows& crowd) {
  const std::size_t width = crowd.empty() ? 0 : crowd.front().size();
  std::vector<int> skyline(width, static_cast<int>(crowd.size()));
  for (std::size_t column = 0; column < width; ++column) {
    for (std::size_t row = 0; row < crowd.size(); ++row) {
      if (crowd[row][column]) {
        skyline[column] = static_cast<int>(row);
        break;
      }
    }
  }
  return skyline;
}

/** @returns Whether column @p column of @p skyline is a dip between two
 * heads: no higher than its neighbours, and HALL_DIP_DEPTH under the highest
 * head within HALL_DIP_REACH columns. */
bool isDip(const std::vector<int>& skyline, std::size_t column) {
  const int depth = skyline[column];
  if (depth < skyline[column - 1] || depth < skyline[column + 1]) {
    return false;
  }
  const std::size_t reach = static_cast<std::size_t>(HALL_DIP_REACH);
  const std::size_t first = column > reach ? column - reach : 0;
  const std::size_t last = std::min(skyline.size(), column + reach + 1);
  const int highest = *std::ranges::min_element(
      skyline.begin() + static_cast<std::ptrdiff_t>(first),
      skyline.begin() + static_cast<std::ptrdiff_t>(last));
  return depth >= highest + HALL_DIP_DEPTH;
}

/** @returns The columns the crowd is cut at, the first and one past the last
 * included: each dip of @p skyline at least HALL_SPECTATOR_WIDTH after the
 * previous cut. */
std::vector<std::size_t> cutsOf(const std::vector<int>& skyline) {
  std::vector<std::size_t> cuts{0};
  for (std::size_t column = 2; column + 2 < skyline.size(); ++column) {
    if (isDip(skyline, column) &&
        column - cuts.back() >=
            static_cast<std::size_t>(HALL_SPECTATOR_WIDTH)) {
      cuts.push_back(column);
    }
  }
  cuts.push_back(skyline.size());
  return cuts;
}

/** @returns The spectator of @p crowd that stands in @p columns, its pixels
 * read in @p picture and its beat set by its place in the crowd. */
Spectator spectatorOf(const sf::Image& picture, const CrowdRows& crowd,
                      SpectatorColumns columns) {
  Spectator spectator{
      .raised = {},
      .phase = static_cast<float>(columns.place) * HALL_BEAT_SPREAD,
      .speed =
          HALL_SLOWEST_HOP +
          (static_cast<float>(scrambledOf(columns.place) % HALL_HOP_SHARES) /
           static_cast<float>(HALL_HOP_SHARES) * HALL_HOP_SPEED_RANGE)};
  for (std::size_t row = 0; row < crowd.size(); ++row) {
    for (std::size_t column = columns.from; column < columns.to; ++column) {
      if (!crowd[row][column]) {
        continue;
      }
      const sf::Vector2i pixel(static_cast<int>(column),
                               HALL_CROWD_TOP + static_cast<int>(row));
      const sf::Color color = colorAt(picture, pixel);
      for (std::size_t lift = 0; lift < spectator.raised.size(); ++lift) {
        spectator.raised.at(lift).push_back(ColoredPixel{
            .position = {pixel.x, pixel.y - static_cast<int>(lift) - 1},
            .color = color});
      }
    }
  }
  return spectator;
}

}  // namespace

std::vector<Spectator> spectatorsOf(const sf::Image& picture) {
  const CrowdRows crowd = crowdOf(picture);
  const std::vector<std::size_t> cuts = cutsOf(skylineOf(crowd));
  std::vector<Spectator> spectators;
  for (std::size_t place = 0; place + 1 < cuts.size(); ++place) {
    spectators.push_back(spectatorOf(
        picture, crowd,
        SpectatorColumns{.from = cuts[place],
                         .to = cuts[place + 1],
                         .place = static_cast<std::uint32_t>(place)}));
  }
  return spectators;
}

HallPicture hallPictureOf(const sf::Image& picture) {
  if (sf::Vector2i(picture.getSize()) != INTERFACE_AREA.size) {
    return HallPicture{};
  }
  HallPicture hall;
  hall.crystals = crystalBeats();
  const sf::Vector2u size = picture.getSize();
  for (unsigned y = 0; y < size.y; ++y) {
    for (unsigned x = 0; x < size.x; ++x) {
      if (isHallBulb(picture.getPixel({x, y}))) {
        fileLight(hall, sf::Vector2i(sf::Vector2u(x, y)));
      }
    }
  }
  hall.frontRopes = frontRopesOf(picture);
  hall.spectators = spectatorsOf(picture);
  return hall;
}

}  // namespace rtype::client
