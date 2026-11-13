#pragma once

#include <cstdint>
#include "SpriteFrame.hpp"

namespace rtype::client::testing {

/** @brief The standard hull, the first and the last hulls of the catalog, an
 * accessory, and a number past the catalog. */
constexpr std::uint8_t STANDARD_HULL = 0;
constexpr std::uint8_t WAR_SHARK_HULL = 26;
constexpr std::uint8_t GOLDEN_BYDO_HULL = 30;
constexpr std::uint8_t CROWN_NUMBER_AS_A_HULL = 3;
constexpr std::uint8_t PAST_THE_CATALOG = 31;

/** @brief The third pilot color, and a color no pilot has. */
constexpr int THIRD_COLOR = 2;
constexpr int NO_PILOT_COLOR = 4;

/** @brief The second frame of the ship of the first color, 34 pixels right
 * of the first; rows of both sheets are 18 pixels apart. */
constexpr game::SpriteFrame SECOND_SHIP_FRAME{
    .left = 34, .top = 0, .width = 32, .height = 16};
/** @brief The top of the third color row of the entity sheet (2 x 18), of
 * the war shark (row 2 of block 0) and of the golden Bydo (block 4, so row
 * 18: 18 x 18). */
constexpr int THIRD_COLOR_TOP = 36;
constexpr int GOLDEN_BYDO_THIRD_COLOR_TOP = 324;

}  // namespace rtype::client::testing
