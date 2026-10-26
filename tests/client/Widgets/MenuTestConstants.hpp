#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client::testing {

/** @brief The buttons of the page of three, A, B and C, by their order. */
constexpr std::size_t FIRST = 0;
constexpr std::size_t SECOND = 1;
constexpr std::size_t LAST = 2;

/** @brief The keys that press and cancel. */
constexpr sf::Keyboard::Key ENTER = sf::Keyboard::Key::Enter;
constexpr sf::Keyboard::Key ESCAPE = sf::Keyboard::Key::Escape;

/** @brief A press held the shortest time less than PRESS_DURATION. */
constexpr engine::Duration JUST_BEFORE_DUE = PRESS_DURATION - ONE_TICK;

/** @brief Steps of one unit of the playfield or one pixel of the picture,
 * and a point inside a bar from its top-left corner. */
constexpr sf::Vector2i ONE_UNIT_LEFT(-1, 0);
constexpr sf::Vector2i ONE_PIXEL_LOWER(0, 1);
constexpr sf::Vector2i ONE_PIXEL_HIGHER(0, -1);
constexpr sf::Vector2i INSIDE_A_BAR(4, 4);

/** @brief The bars of the form, NAME then OK, by their order, and the size
 * of its field. */
constexpr std::size_t FIELD = 0;
constexpr std::size_t BUTTON_AFTER_THE_FIELD = 1;
constexpr std::size_t FOUR_BYTES = 4;

/** @brief Width of every character of the regular font, in pixels, read from
 * its file. */
constexpr int CHARACTER_WIDTH = 8;

/**
 * @brief The caret the team chose: one pixel wide, and taller than a line of
 * text by a pixel above and below it.
 */
constexpr sf::Vector2i THIN_CARET_SIZE(1, 10);

/** @brief Characters of the texts the field holds in the tests. */
constexpr int CHARACTERS_OF_AB = 2;
constexpr int CHARACTERS_OF_AB_AND_E_ACUTE = 3;

/** @brief The letter e with an acute accent, one character of two bytes, and
 * the text AB followed by it, in UTF-8. */
constexpr char32_t E_ACUTE = U'\xE9';
constexpr const char* AB_AND_E_ACUTE = "AB\xC3\xA9";

/** @brief The tabs of the tabbed panel by their order, and its row whose
 * control can be activated. */
constexpr std::size_t SECOND_TAB = 1;
constexpr std::size_t LAST_TAB = 2;
constexpr std::size_t LINK_ROW = 1;

}  // namespace rtype::client::testing
