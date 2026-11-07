#pragma once

#include <cstdint>
#include <string>
#include "ActionKey.hpp"
#include "DataFilePlace.hpp"

namespace rtype::game {

/**
 * @brief One table `[[gameplay.volley]]` of the sheet of an enemy: the bodies
 * that appear at once when the enemy enters the action `on`.
 *
 * Body `index`, from 0 to count - 1, steers from the start (offsetX, offsetY
 * from the Position of the enemy) moved index times by (stepX, stepY), and
 * appears `distance` units from that point along its heading: left, turned by
 * an angle that runs evenly from -spreadDegrees / 2 to +spreadDegrees / 2
 * over the volley, a positive angle turning it down. A fan of scatter
 * missiles is a spread and a distance with no step; a column of Bydos is a
 * step with no spread; the bodies from gapFrom to gapFrom + gapCount - 1 are
 * left out, the gap of a wall (see volleyPlacement()).
 */
struct VolleySheet {
  ActionKey on{};
  std::uint8_t count{0};
  float offsetX{0.0F};
  float offsetY{0.0F};
  float stepX{0.0F};
  float stepY{0.0F};
  float spreadDegrees{0.0F};
  float distance{0.0F};
  /** @brief The heading the fan turns around, in degrees from the left,
   * a positive angle turning down. */
  float headingDegrees{0.0F};
  /** @brief How far the start moves at random each time it fires, on
   * each axis (see VolleySystem). */
  float jitterX{0.0F};
  float jitterY{0.0F};
  std::uint8_t gapFrom{0};
  std::uint8_t gapCount{0};
  /** @brief The chance it fires when `on` starts, in percent: 100 fires
   * it every time (see VolleySystem). */
  std::uint8_t chancePercent{100};
  /** @brief The name of the sheet of its bodies, as the file writes it. */
  std::string body;
  /** @brief The number of that sheet; 0 until EntitySheets checked that it
   * exists, of the side of an enemy or of its missiles, and numbered it. */
  std::uint8_t bodyNumber{0};
  /** @brief Where the file names that sheet, for the errors of EntitySheets. */
  engine::DataFilePlace bodyPlace;
};

}  // namespace rtype::game
