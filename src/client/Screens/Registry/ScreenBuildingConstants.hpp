#pragma once

#include <array>
#include "TextSize.hpp"

namespace rtype::client {

/** @brief The half pixels a font pixel covers at each text size, in the
 * order of TEXT_SIZES: as the fonts are drawn, half again as large, twice as
 * large. */
constexpr std::array<int, TEXT_SIZE_COUNT> TEXT_SIZE_HALF_PIXELS{2, 3, 4};

}  // namespace rtype::client
