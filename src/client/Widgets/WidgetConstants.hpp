#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include "InterfaceConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Margin every menu keeps free along the edges of the interface
 * picture, in pixels. */
constexpr int SAFE_MARGIN = 24;

/**
 * @brief A box has two sides along each axis: centering it splits the free
 * space in that many margins, and a border is taken that many times from its
 * size.
 */
constexpr int SIDES_PER_AXIS = 2;

/**
 * @brief Column of a page, from the top-left corner of the safe margin
 * downwards: the space left under each part, in pixels, as on the mockup of
 * the title screen. The rule is drawn under the last part of the header.
 */
constexpr int SPACE_BELOW_VERSION = 4;
constexpr int SPACE_BELOW_HEADING = 8;
constexpr int SPACE_BELOW_SUBTITLE = 6;
constexpr sf::Vector2i RULE_SIZE(240, 1);
constexpr int SPACE_BELOW_RULE = 9;

/** @brief Bars of a page: their size, and the space between two of them, in
 * pixels. */
constexpr sf::Vector2i BUTTON_SIZE(160, 20);
constexpr int BUTTON_GAP = 8;

/**
 * @brief Where the label of a bar starts, from its top-left corner, and how
 * many pixels it drops while the bar is pressed.
 */
constexpr sf::Vector2i BUTTON_TEXT_OFFSET(12, 6);
constexpr int PRESSED_TEXT_DROP = 1;

/**
 * @brief Check box of a button that is one choice among others: a square at
 * the start of its label, standing on the baseline, its inside filled but for
 * a margin when the choice is the current one, then a gap before the label.
 */
constexpr int CHECK_BOX_SIZE = 7;
constexpr int CHECK_MARK_MARGIN = 2;
constexpr int CHECK_BOX_GAP = 4;

/**
 * @brief Corners cut off the bars and panels: rows of the outer cut, and
 * thickness of the border, the inside being cut one row less.
 */
constexpr int CHAMFER_ROWS = 4;
constexpr int BORDER_THICKNESS = 1;

/** @brief Row of the top of the illustration of a page, against the right
 * side of the safe margin. */
constexpr int ILLUSTRATION_TOP = 64;

/** @brief Row of the top of the line of the footer of a page, in its
 * bottom-right corner. */
constexpr int FOOTER_TOP = 241;

/**
 * @brief Dialog: size of its panel, centered on the picture, and where its
 * parts sit from the top-left corner of the panel; its buttons form a
 * centered row.
 */
constexpr sf::Vector2i DIALOG_SIZE(272, 96);
constexpr sf::Vector2i DIALOG_HEADING_OFFSET(16, 12);
constexpr sf::Vector2i DIALOG_MESSAGE_OFFSET(16, 32);
constexpr int DIALOG_BUTTONS_TOP = 56;
constexpr sf::Vector2i DIALOG_BUTTON_SIZE(64, 20);
constexpr int DIALOG_BUTTON_GAP = 32;

/**
 * @brief Form: a panel centered on the picture, as wide as the column of a
 * page and as tall as its parts. Its heading sits where a dialog has it; its
 * fields stand one under the other, as far from the left side of the panel as
 * the heading; its buttons, half as wide as those of a page, form a centered
 * row; the description of the selected bar and the status line close it.
 * Spaces in pixels, as on the picture of the connection screen the team chose.
 */
constexpr int FORM_WIDTH = RULE_SIZE.x;
constexpr sf::Vector2i FORM_HEADING_OFFSET = DIALOG_HEADING_OFFSET;
constexpr int SPACE_BELOW_FORM_HEADING = 12;
constexpr int SPACE_BETWEEN_FIELDS = 4;
constexpr int SPACE_AROUND_FORM_BUTTONS = 10;
constexpr sf::Vector2i FORM_BUTTON_SIZE(BUTTON_SIZE.x / SIDES_PER_AXIS,
                                        BUTTON_SIZE.y);
constexpr int FORM_BUTTON_GAP = BUTTON_GAP;

/** @brief Shortest time a button looks pressed before its action runs, so
 * even a quick click is seen. */
constexpr engine::Duration PRESS_DURATION = std::chrono::milliseconds(140);

/**
 * @brief Keys of a menu: the next and previous buttons, Tab (Shift+Tab goes
 * back), the key that activates the selected button and the one that
 * activates the cancel button.
 */
constexpr std::array MENU_NEXT_KEYS{sf::Keyboard::Key::Down,
                                    sf::Keyboard::Key::Right};
constexpr std::array MENU_PREVIOUS_KEYS{sf::Keyboard::Key::Up,
                                        sf::Keyboard::Key::Left};
constexpr sf::Keyboard::Key MENU_TAB_KEY = sf::Keyboard::Key::Tab;
constexpr sf::Keyboard::Key MENU_CONFIRM_KEY = sf::Keyboard::Key::Enter;
constexpr sf::Keyboard::Key MENU_CANCEL_KEY = sf::Keyboard::Key::Escape;

/**
 * @brief Text field: its label in small print, then, FIELD_LABEL_GAP lower, a
 * box sunk into the panel, as wide as a form leaves between the left side of
 * its heading and its right side, its text FIELD_TEXT_PADDING inside. While the
 * field is selected, a caret one pixel wide follows the text, taller than the
 * line by FIELD_CARET_OVERHANG above and below it.
 */
constexpr int FIELD_LABEL_GAP = 2;
constexpr int FIELD_TEXT_PADDING = 4;
constexpr int FIELD_CARET_OVERHANG = 1;
constexpr sf::Vector2i FIELD_BOX_SIZE(
    FORM_WIDTH - (SIDES_PER_AXIS * FORM_HEADING_OFFSET.x),
    TEXT_LINE_HEIGHT + (SIDES_PER_AXIS * FIELD_TEXT_PADDING));
constexpr sf::Vector2i FIELD_SIZE(FIELD_BOX_SIZE.x, TEXT_LINE_HEIGHT +
                                                        FIELD_LABEL_GAP +
                                                        FIELD_BOX_SIZE.y);
constexpr sf::Vector2i FIELD_CARET_SIZE(
    1, TEXT_LINE_HEIGHT + (SIDES_PER_AXIS * FIELD_CARET_OVERHANG));

/**
 * @brief Most characters the box of a field shows with the caret after them,
 * the regular font giving each REGULAR_FONT_ADVANCE pixels: a screen keeps the
 * size of each of its fields, in bytes, within it.
 */
constexpr std::size_t FIELD_CHARACTER_CAPACITY = static_cast<std::size_t>(
    (FIELD_BOX_SIZE.x - (SIDES_PER_AXIS * FIELD_TEXT_PADDING) -
     FIELD_CARET_SIZE.x) /
    REGULAR_FONT_ADVANCE);

/** @brief Key that erases the last character of the selected text field. */
constexpr sf::Keyboard::Key FIELD_ERASE_KEY = sf::Keyboard::Key::Backspace;

/**
 * @brief The characters a text field may hold: Basic Latin from the space to
 * the tilde, and Latin-1 Supplement from the no-break space to U+00FF. Both
 * interface fonts draw each of them, one em wide in the regular font (read from
 * their files); no control character is among them.
 */
constexpr char32_t FIRST_PRINTABLE_BASIC_LATIN = U' ';
constexpr char32_t LAST_PRINTABLE_BASIC_LATIN = U'~';
constexpr char32_t FIRST_PRINTABLE_LATIN1_SUPPLEMENT = U'\xA0';
constexpr char32_t LAST_PRINTABLE_LATIN1_SUPPLEMENT = U'\xFF';

/** @brief The decimal digits. */
constexpr char32_t FIRST_DIGIT = U'0';
constexpr char32_t LAST_DIGIT = U'9';

/**
 * @brief Space between the description of the selected bar of a form and its
 * status line, in pixels: as far apart as two bars of a page.
 */
constexpr int SPACE_BELOW_DESCRIPTION = BUTTON_GAP;

/** @brief Label of the button that closes a menu, the same in every menu. */
constexpr const char* BACK_LABEL = "BACK";

}  // namespace rtype::client
