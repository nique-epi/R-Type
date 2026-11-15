#pragma once

#include "ClothBand.hpp"
#include "ClothPlace.hpp"

namespace rtype::client::testing {

/** @brief Places of the band of the numbers, counted across from the zero, a
 * number then a line, and down from the top row: 17, the line after it, the
 * line under it; 1, the street edge under it; 2 and the zero come from the
 * middle row. */
constexpr ClothPlace SEVENTEEN_PLACE{.across = 11, .down = 2};
constexpr ClothPlace AFTER_SEVENTEEN{.across = 12, .down = 2};
constexpr ClothPlace UNDER_SEVENTEEN{.across = 11, .down = 3};
constexpr ClothPlace ONE_PLACE{.across = 1, .down = 4};
constexpr ClothPlace UNDER_ONE{.across = 1, .down = 5};
constexpr ClothPlace TWO_PLACE{.across = 1, .down = 2};
constexpr ClothPlace ZERO_FROM_THE_MIDDLE{.across = 0, .down = 2};
constexpr ClothPlace THREE_PLACE{.across = 1, .down = 0};

/** @brief 35 on the middle row of the last street, the line under it, the
 * column of its row, and the column of the bottom row. */
constexpr ClothPlace THIRTY_FIVE_PLACE{.across = 23, .down = 2};
constexpr ClothPlace UNDER_THIRTY_FIVE{.across = 23, .down = 3};
constexpr ClothPlace MIDDLE_COLUMN{.across = 24, .down = 2};
constexpr ClothPlace BOTTOM_COLUMN{.across = 24, .down = 4};

/** @brief The dozens and the even chances under them. */
constexpr ClothPlace FIRST_DOZEN{.band = ClothBand::Dozens, .across = 0};
constexpr ClothPlace THIRD_DOZEN{.band = ClothBand::Dozens, .across = 2};
constexpr ClothPlace LOW_CHANCE{.band = ClothBand::EvenChances, .across = 0};
constexpr ClothPlace EVEN_CHANCE{.band = ClothBand::EvenChances, .across = 1};
constexpr ClothPlace HIGH_CHANCE{.band = ClothBand::EvenChances, .across = 5};

}  // namespace rtype::client::testing
