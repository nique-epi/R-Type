#pragma once

#include <SFML/Graphics/Color.hpp>
#include "BackgroundColors.hpp"
#include "InterfaceColors.hpp"

namespace rtype::client {

/** @brief A bar at rest: a blue border around the night sky, pale text. */
constexpr sf::Color BAR_BORDER_COLOR = EDGE_BLUE;
constexpr sf::Color BAR_INSIDE_COLOR = SKY_COLOR;
constexpr sf::Color BAR_TEXT_COLOR = PALE_BLUE;

/**
 * @brief A selected bar, by the keyboard or under the pointer: it fills with
 * blue and its text turns dark, a change of fill that reads without color.
 */
constexpr sf::Color SELECTED_BAR_BORDER_COLOR = HUD_CYAN;
constexpr sf::Color SELECTED_BAR_INSIDE_COLOR = EDGE_BLUE;
constexpr sf::Color SELECTED_BAR_TEXT_COLOR = INK;

/** @brief A pressed bar: dark inside, gold text dropped by
 * PRESSED_TEXT_DROP. */
constexpr sf::Color PRESSED_BAR_INSIDE_COLOR(15, 90, 115);
constexpr sf::Color PRESSED_BAR_TEXT_COLOR = STAR_GOLD;

/** @brief A disabled bar: its border is turned off. */
constexpr sf::Color DISABLED_BAR_BORDER_COLOR = DEEP_BLUE;
constexpr sf::Color DISABLED_BAR_TEXT_COLOR = BLUE;

/** @brief The rule under the header of a page, and the panel of a dialog. */
constexpr sf::Color RULE_COLOR = EDGE_BLUE;
constexpr sf::Color PANEL_BORDER_COLOR = EDGE_BLUE;
constexpr sf::Color PANEL_INSIDE_COLOR = SKY_COLOR;

/**
 * @brief Lines of a page, laid on the sky; title and question of a dialog,
 * inside its panel.
 */
constexpr sf::Color PAGE_TEXT_COLOR = HUD_CYAN;
constexpr sf::Color DIALOG_HEADING_COLOR = HUD_CYAN;
constexpr sf::Color DIALOG_MESSAGE_COLOR = STAR_WHITE;

}  // namespace rtype::client
