#pragma once

#include <span>
#include <string_view>
#include "MaskPixel.hpp"
#include "PixelMask.hpp"

/** @returns The mask @p rows draw: one row each, DRAWN_TEST_PIXEL drawn and
 * anything else transparent, as wide as the first row. */
[[nodiscard]] rtype::engine::PixelMask maskFromRows(
    std::span<const std::string_view> rows);

/** @returns A mask of @p width by @p height with every pixel drawn. */
[[nodiscard]] rtype::engine::PixelMask fullMask(int width, int height);

/** @returns A mask one row of @p width tall with @p pixel alone drawn. */
[[nodiscard]] rtype::engine::PixelMask rowDrawnAt(
    int width, rtype::engine::MaskPixel pixel);
