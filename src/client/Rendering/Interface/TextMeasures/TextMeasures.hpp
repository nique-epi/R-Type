#pragma once

#include <string_view>
#include "TextMetrics.hpp"
#include "TextSize.hpp"

namespace rtype::client {

/** @returns The height of a line of text at @p metrics, in pixels of the
 * picture: 8 as the fonts are drawn, 12 half again as large, 16 twice. */
[[nodiscard]] int lineHeightOf(TextMetrics metrics);

/** @returns The width of @p text in the regular font at @p metrics, in pixels
 * of the picture: the font gives every character an advance of one line. */
[[nodiscard]] int regularWidthOf(TextMetrics metrics, std::string_view text);

/** @returns The width at @p metrics of a line @p nativeWidth pixels wide as
 * the fonts are drawn, rounded up to a whole pixel. */
[[nodiscard]] int widthAtSizeOf(TextMetrics metrics, int nativeWidth);

/** @returns The half pixels each font pixel of a line of @p styleScale covers:
 * the larger of its own scale and the size of @p metrics, so a title written
 * twice as large is not made larger again. */
[[nodiscard]] int halfPixelsOf(TextMetrics metrics, int styleScale);

/** @returns The metrics of a text size of the options: two, three or four
 * half pixels per font pixel. */
[[nodiscard]] TextMetrics textMetricsOf(TextSize size);

}  // namespace rtype::client
