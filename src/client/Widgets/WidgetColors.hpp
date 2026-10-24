#pragma once

#include <SFML/Graphics/Color.hpp>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"

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

/** @brief Slider: its track and empty part (3.1:1 on INK), its filled part
 * at rest and on the selected row, its thumb at rest and on the selected row,
 * and the outline of the thumb. */
constexpr sf::Color SLIDER_TRACK_COLOR = BLUE;
constexpr sf::Color SLIDER_FILL_COLOR = PALE_BLUE;
constexpr sf::Color SELECTED_SLIDER_FILL_COLOR = HUD_CYAN;
constexpr sf::Color SLIDER_THUMB_COLOR = PALE_BLUE;
constexpr sf::Color SELECTED_SLIDER_THUMB_COLOR = STAR_WHITE;
constexpr sf::Color SLIDER_THUMB_OUTLINE_COLOR = INK;

/** @brief The current cell of a switch, at rest and on the selected row, and
 * its text; the other cell's text. */
constexpr sf::Color CURRENT_CELL_COLOR = PALE_BLUE;
constexpr sf::Color SELECTED_CURRENT_CELL_COLOR = HUD_CYAN;
constexpr sf::Color CURRENT_CELL_TEXT_COLOR = INK;
constexpr sf::Color OTHER_CELL_TEXT_COLOR = FIELD_TEXT_COLOR;

/** @brief A value inside a sunk box, at rest and on the selected row; a value
 * picked but not applied, or a key awaited. */
constexpr sf::Color BOXED_VALUE_COLOR = FIELD_TEXT_COLOR;
constexpr sf::Color SELECTED_BOXED_VALUE_COLOR = STAR_WHITE;
constexpr sf::Color AWAITED_VALUE_COLOR = STAR_GOLD;

/** @brief The open list of a dropdown: its border and inside, its
 * highlighted line and the text on it, a choice that cannot be taken. */
constexpr sf::Color LIST_BORDER_COLOR = HUD_CYAN;
constexpr sf::Color LIST_INSIDE_COLOR = INK;
constexpr sf::Color LIST_HIGHLIGHT_COLOR = EDGE_BLUE;
constexpr sf::Color LIST_HIGHLIGHT_TEXT_COLOR = INK;
constexpr sf::Color UNAVAILABLE_CHOICE_COLOR = BLUE;

/** @brief Keycap: its face, edge, lit side and name; its edge on the
 * selected row and on a conflict; the conflict flag. */
constexpr sf::Color KEY_CAP_FACE_COLOR = INK;
constexpr sf::Color KEY_CAP_EDGE_COLOR = PALE_BLUE;
constexpr sf::Color KEY_CAP_SIDE_COLOR = BLUE;
constexpr sf::Color KEY_CAP_TEXT_COLOR = STAR_WHITE;
constexpr sf::Color SELECTED_KEY_CAP_EDGE_COLOR = HUD_CYAN;
constexpr sf::Color CONFLICT_KEY_CAP_EDGE_COLOR = STAR_GOLD;
constexpr sf::Color CONFLICT_FLAG_COLOR = STAR_GOLD;

/** @brief Tabs: the border and label of the open tab, of the others. */
constexpr sf::Color OPEN_TAB_BORDER_COLOR = HUD_CYAN;
constexpr sf::Color OPEN_TAB_TEXT_COLOR = HUD_CYAN;
constexpr sf::Color TAB_BORDER_COLOR = EDGE_BLUE;
constexpr sf::Color TAB_TEXT_COLOR = PALE_BLUE;
constexpr sf::Color TAB_INSIDE_COLOR = SKY_COLOR;

}  // namespace rtype::client
