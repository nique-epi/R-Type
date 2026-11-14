#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "TabbedPanelLayout.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/**
 * @returns The layout of a tabbed panel at @p metrics: as the options were
 * drawn at the size the fonts are drawn, and at a larger size, every box that
 * holds a line grown with it, the bars and the two lines of the description
 * still ending SAFE_MARGIN over the bottom, and the panel taking what is
 * left in between.
 */
[[nodiscard]] TabbedPanelLayout tabbedPanelLayoutOf(TextMetrics metrics);

/** @returns The area the rows of @p layout show in, as many as fit. */
[[nodiscard]] sf::IntRect rowsViewportOf(const TabbedPanelLayout& layout);

}  // namespace rtype::client
