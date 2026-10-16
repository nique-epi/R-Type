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

/**
 * @brief A text field: a box sunk into its panel, dark inside, its top and
 * left sides in shade and its bottom and right sides lit; its label above the
 * box and its text are pale.
 */
constexpr sf::Color FIELD_INSIDE_COLOR = INK;
constexpr sf::Color FIELD_SHADED_SIDE_COLOR = DEEP_BLUE;
constexpr sf::Color FIELD_LIT_SIDE_COLOR = BLUE;
constexpr sf::Color FIELD_TEXT_COLOR = PALE_BLUE;

/**
 * @brief The selected text field, the one the keyboard types into: its label
 * and its four sides light up in cyan, its text turns white, and a cyan caret
 * follows the text.
 */
constexpr sf::Color SELECTED_FIELD_LABEL_COLOR = HUD_CYAN;
constexpr sf::Color SELECTED_FIELD_BORDER_COLOR = HUD_CYAN;
constexpr sf::Color SELECTED_FIELD_TEXT_COLOR = STAR_WHITE;
constexpr sf::Color FIELD_CARET_COLOR = HUD_CYAN;

/**
 * @brief The lines that close a form: the description of the selected bar in
 * cyan, as on a page, then the status line in gold, as the label of a pressed
 * bar, so it stands apart from the description; its text says what it means,
 * not its color.
 */
constexpr sf::Color FORM_DESCRIPTION_COLOR = HUD_CYAN;
constexpr sf::Color STATUS_TEXT_COLOR = STAR_GOLD;

}  // namespace rtype::client
