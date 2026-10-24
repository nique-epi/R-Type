#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

/**
 * @brief One pixel of the chart stands for WINDOW_SIZE_CHART_SCALE pixels of
 * a window, so the chart is as large as the last, largest size of
 * WINDOW_SIZES.
 */
constexpr unsigned int WINDOW_SIZE_CHART_SCALE = 20;
constexpr sf::Vector2i WINDOW_SIZE_CHART_SIZE(
    static_cast<int>(WINDOW_SIZES.back().width / WINDOW_SIZE_CHART_SCALE),
    static_cast<int>(WINDOW_SIZES.back().height / WINDOW_SIZE_CHART_SCALE));

/**
 * @brief The legend: WINDOW_SIZE_LEGEND_GAP right of the chart, its first
 * line WINDOW_SIZE_LEGEND_OFFSET from the top-left corner of the chart, one
 * line every WINDOW_SIZE_LEGEND_LINE_HEIGHT pixels, each a tag then a size.
 */
constexpr int WINDOW_SIZE_LEGEND_GAP = 12;
constexpr sf::Vector2i WINDOW_SIZE_LEGEND_OFFSET(WINDOW_SIZE_CHART_SIZE.x +
                                                     WINDOW_SIZE_LEGEND_GAP,
                                                 8);
constexpr int WINDOW_SIZE_LEGEND_LINE_HEIGHT = 12;
constexpr const char* CURRENT_SIZE_TAG = "NOW  ";
constexpr const char* CANDIDATE_SIZE_TAG = "NEW  ";
constexpr const char* TOO_LARGE_SIZE_TAG = "TOO LARGE  ";

/**
 * @brief Outlines of the sizes: any size, the size of the window, a size
 * picked but not applied yet, a size too large for the desktop, dashed; and
 * the legend line of a size too large.
 */
constexpr sf::Color SIZE_OUTLINE_COLOR = DEEP_BLUE;
constexpr sf::Color CURRENT_SIZE_COLOR = HUD_CYAN;
constexpr sf::Color CANDIDATE_SIZE_COLOR = STAR_GOLD;
constexpr sf::Color TOO_LARGE_SIZE_COLOR = BLUE;
constexpr sf::Color TOO_LARGE_LEGEND_COLOR = PALE_BLUE;

}  // namespace rtype::client
