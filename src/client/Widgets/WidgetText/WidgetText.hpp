#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string_view>
#include "BarState.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/** @returns The left of a line of text in the regular font at @p metrics
 * centered in a box, in pixels. */
[[nodiscard]] int centeredTextLeft(sf::IntRect box, std::string_view text,
                                   TextMetrics metrics);

/** @returns The top of a line of text at @p metrics centered in a box: its
 * middle row, in pixels. */
[[nodiscard]] int centeredTextTop(sf::IntRect box, TextMetrics metrics);

/** @returns Where the box of a field of @p area lies at the text size of
 * @p metrics: under its label, as drawn at the size the fonts are drawn at;
 * above it, the whole height of the area, right of a column of
 * FIELD_LABEL_LETTERS letters and FIELD_LABEL_SIDE_GAP where its label
 * stands. */
[[nodiscard]] sf::IntRect fieldBoxOf(sf::IntRect area, TextMetrics metrics);

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
