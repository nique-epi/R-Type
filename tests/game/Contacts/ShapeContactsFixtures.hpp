#pragma once

#include "PixelMask.hpp"

namespace rtype::game::testing {

/** @returns A mask TWO_PIXELS_WIDE by ONE_PIXEL_TALL with LEFT_PIXEL alone
 * drawn. */
[[nodiscard]] engine::PixelMask leftPixelDrawn();

}  // namespace rtype::game::testing
