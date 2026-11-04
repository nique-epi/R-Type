#pragma once

#include <cstddef>

namespace rtype::client::testing {

/** @brief An id that breaks the asset id rule, and a font whose file holds no
 * font. */
constexpr const char* BADLY_NAMED_ID = "sprites/Bydo.png";
constexpr const char* CORRUPT_FONT_ID = "fonts/corrupt.ttf";

/** @brief Counts of the assets a step processes. */
constexpr std::size_t ONE_ASSET = 1;
constexpr std::size_t TWO_ASSETS = 2;

}  // namespace rtype::client::testing
