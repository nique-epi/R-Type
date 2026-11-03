#pragma once

#include <cstddef>
#include "PixelPosition.hpp"
#include "PixelSize.hpp"

namespace rtype::client::testing {

/** @brief A 1920 x 1080 desktop: 1600 x 900 is the largest size that fits, 160
 * pixels from its left and 90 from its top once centered. */
constexpr PixelSize DESKTOP_1920_BY_1080{.width = 1920, .height = 1080};
constexpr PixelSize LARGEST_SIZE_THAT_FITS{.width = 1600, .height = 900};
constexpr PixelPosition LARGEST_SIZE_CENTERED_AT{.x = 160, .y = 90};

/** @brief Where a borderless window starts: the corner of the desktop. */
constexpr PixelPosition DESKTOP_TOP_LEFT{.x = 0, .y = 0};

/** @brief A size the player saved, and the order of the smallest size of the
 * list. */
constexpr PixelSize SAVED_SIZE{.width = 1280, .height = 720};
constexpr std::size_t SMALLEST_SIZE_INDEX = 0;

}  // namespace rtype::client::testing
