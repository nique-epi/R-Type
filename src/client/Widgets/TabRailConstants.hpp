#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "Translation.hpp"

namespace rtype::client {

/**
 * @brief A rail of text tabs, as the design system draws its Tab Rail v2: the
 * labels TAB_RAIL_GAP apart on the sky, with no line under them; the open one
 * underlined by a bar TAB_RAIL_INDICATOR_TOP under the top of its label,
 * TAB_RAIL_INDICATOR_INSET shorter than the label on each side, with a dot
 * TAB_RAIL_DOT_TOP under that top, centred over the bar. A pointer opens a
 * tab anywhere between TAB_RAIL_HIT_MARGIN over the label and the bottom of
 * the bar.
 */
constexpr int TAB_RAIL_GAP = 20;
constexpr int TAB_RAIL_INDICATOR_TOP = TEXT_LINE_HEIGHT + 4;
constexpr int TAB_RAIL_INDICATOR_HEIGHT = 2;
constexpr int TAB_RAIL_INDICATOR_INSET = 4;
constexpr int TAB_RAIL_DOT_TOP = TEXT_LINE_HEIGHT + 1;
constexpr int TAB_RAIL_DOT_SIZE = 2;
constexpr int TAB_RAIL_HIT_MARGIN = 4;

/** @brief The colors of the labels: the open tab and a tab under the pointer
 * in star white, the others in pale blue, a locked tab in blue, the bar and
 * its dot in the cyan of the interface. */
constexpr sf::Color TAB_RAIL_OPEN_COLOR = STAR_WHITE;
constexpr sf::Color TAB_RAIL_IDLE_COLOR = PALE_BLUE;
constexpr sf::Color TAB_RAIL_LOCKED_COLOR = BLUE;
constexpr sf::Color TAB_RAIL_INDICATOR_COLOR = HUD_CYAN;

/** @brief The badge after the label of a locked tab: TAB_RAIL_BADGE_TEXT in
 * small print, ink on orange, in a box TAB_RAIL_BADGE_SIZE large,
 * TAB_RAIL_BADGE_GAP after the label and TAB_RAIL_BADGE_RISE above its top,
 * the text TAB_RAIL_BADGE_TEXT_DROP under the top of the box. */
constexpr Translation TAB_RAIL_BADGE_TEXT{.english = "SOON",
                                          .french = "BIENTÔT"};
constexpr sf::Vector2i TAB_RAIL_BADGE_SIZE{28, 9};
constexpr int TAB_RAIL_BADGE_GAP = 4;
constexpr int TAB_RAIL_BADGE_RISE = 1;
constexpr int TAB_RAIL_BADGE_TEXT_DROP = 1;
constexpr sf::Color TAB_RAIL_BADGE_COLOR = WARNING_ORANGE;
constexpr sf::Color TAB_RAIL_BADGE_TEXT_COLOR = INK;

/** @brief How a locked tab refuses: it shakes TAB_RAIL_REFUSAL_SHIFT pixels
 * left then right, TAB_RAIL_REFUSAL_STEPS steps over TAB_RAIL_REFUSAL_TIME. */
constexpr std::chrono::milliseconds TAB_RAIL_REFUSAL_TIME{280};
constexpr int TAB_RAIL_REFUSAL_STEPS = 4;
constexpr int TAB_RAIL_REFUSAL_SHIFT = 2;

}  // namespace rtype::client
