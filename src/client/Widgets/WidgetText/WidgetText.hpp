#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string_view>
#include "BarState.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/** @returns How wide a line of UTF-8 text is in the regular font at
 * @p metrics, in pixels: one line height per character. */
[[nodiscard]] int regularTextWidth(TextMetrics metrics, std::string_view text);

/** @returns The left of a line of text in the regular font at @p metrics
 * centered in a box, in pixels. */
[[nodiscard]] int centeredTextLeft(sf::IntRect box, std::string_view text,
                                   TextMetrics metrics);

/** @returns The top of a line of text at @p metrics centered in a box: its
 * middle row, in pixels. */
[[nodiscard]] int centeredTextTop(sf::IntRect box, TextMetrics metrics);

/** @returns How tall a box made around one line of the fonts as drawn,
 * @p nativeHeight tall, is at @p metrics: it grows as its line does. */
[[nodiscard]] int textBoxHeight(int nativeHeight, TextMetrics metrics);

/** @returns The text size a row control of @p area was laid out at, read
 * from its height: a control is CONTROL_HEIGHT tall at the size the fonts
 * are drawn, and grows with its line. */
[[nodiscard]] TextMetrics metricsOfControl(sf::IntRect area);

/** @returns The color of the label of a bar in a state, also the color of
 * text laid on a row outside any box. */
[[nodiscard]] sf::Color barTextColor(BarState state);

}  // namespace rtype::client
