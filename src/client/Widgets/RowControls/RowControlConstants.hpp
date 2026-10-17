#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string_view>
#include "InterfaceConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief Keys of the controls: those that step a value down and up, those
 * that move a slider to its ends or by a page, and those that move the
 * highlight of an open list.
 */
constexpr sf::Keyboard::Key DECREASE_KEY = sf::Keyboard::Key::Left;
constexpr sf::Keyboard::Key INCREASE_KEY = sf::Keyboard::Key::Right;
constexpr sf::Keyboard::Key FIRST_VALUE_KEY = sf::Keyboard::Key::Home;
constexpr sf::Keyboard::Key LAST_VALUE_KEY = sf::Keyboard::Key::End;
constexpr sf::Keyboard::Key PAGE_UP_KEY = sf::Keyboard::Key::PageUp;
constexpr sf::Keyboard::Key PAGE_DOWN_KEY = sf::Keyboard::Key::PageDown;
constexpr sf::Keyboard::Key LIST_PREVIOUS_KEY = sf::Keyboard::Key::Up;
constexpr sf::Keyboard::Key LIST_NEXT_KEY = sf::Keyboard::Key::Down;

/**
 * @brief Slider: a sunk groove, its track SLIDER_TRACK_INSET in from both
 * sides, SLIDER_TRACK_THICKNESS thick, a thumb SLIDER_THUMB_SIZE outlined by
 * one pixel, then its value right-aligned on SLIDER_VALUE_WIDTH ("100%").
 */
constexpr sf::Vector2i SLIDER_GROOVE_SIZE(132, CONTROL_HEIGHT);
constexpr int SLIDER_TRACK_INSET = 4;
constexpr int SLIDER_TRACK_LENGTH =
    SLIDER_GROOVE_SIZE.x - (SIDES_PER_AXIS * SLIDER_TRACK_INSET);
constexpr int SLIDER_TRACK_TOP = 5;
constexpr int SLIDER_TRACK_THICKNESS = 2;
constexpr sf::Vector2i SLIDER_THUMB_SIZE(6, 10);
constexpr int SLIDER_THUMB_TOP = 1;
constexpr std::string_view WIDEST_SLIDER_VALUE = "100%";
constexpr int SLIDER_VALUE_WIDTH =
    static_cast<int>(WIDEST_SLIDER_VALUE.size()) * REGULAR_FONT_ADVANCE;
constexpr const char* SLIDER_INACTIVE_TEXT = "OFF";

/** @brief Divides a doubled quotient into the quotient rounded to the nearest
 * whole number, as the step under the pointer is found. */
constexpr int ROUNDING_DIVISOR = 2;

/** @brief Switch: two cells in a sunk box, the current one filled. */
constexpr sf::Vector2i SWITCH_CELL_SIZE(30, 10);
constexpr int SWITCH_CELL_INSET = 1;
constexpr const char* SWITCH_OFF_TEXT = "OFF";
constexpr const char* SWITCH_ON_TEXT = "ON";

/** @brief Selector: an arrow on each side of a sunk box as wide as the
 * longest choice plus SELECTOR_BOX_PADDING. */
constexpr const char* DECREASE_ARROW = "<";
constexpr const char* INCREASE_ARROW = ">";
constexpr int SELECTOR_BOX_PADDING = 8;

/**
 * @brief Dropdown: its box, the text DROPDOWN_TEXT_LEFT in, a pointer of
 * DROPDOWN_POINTER_SIZE DROPDOWN_POINTER_RIGHT from its right side; its list
 * right under the box, as wide, one LIST_ITEM_HEIGHT line per choice inside
 * LIST_PADDING, each with its check box and its text.
 */
constexpr sf::Vector2i DROPDOWN_BOX_SIZE(112, CONTROL_HEIGHT);
constexpr int DROPDOWN_TEXT_LEFT = 4;
constexpr sf::Vector2i DROPDOWN_POINTER_SIZE(7, 4);
constexpr int DROPDOWN_POINTER_RIGHT = 3;
constexpr int DROPDOWN_POINTER_TOP = 4;
constexpr int LIST_ITEM_HEIGHT = 14;
constexpr int LIST_PADDING = 2;
constexpr sf::Vector2i LIST_CHECK_BOX_OFFSET(3, 3);
constexpr sf::Vector2i LIST_TEXT_OFFSET(14, 3);

/** @brief What follows the text of a row that opens something: "RESET >". */
constexpr const char* LINK_SUFFIX = " >";

/** @brief The text of a slot waiting for a key, in a sunk box, and its
 * padding. */
constexpr const char* KEY_LISTENING_TEXT = "PRESS A KEY";
constexpr int KEY_LISTENING_PADDING = 4;
constexpr const char* KEY_CONFLICT_FLAG = "!";

}  // namespace rtype::client
