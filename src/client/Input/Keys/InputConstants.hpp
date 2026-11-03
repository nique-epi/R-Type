#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <string_view>
#include "BoundAction.hpp"

namespace rtype::client {

/**
 * @brief The keys of a first launch, in the order of BOUND_ACTIONS: the
 * arrows move, space shoots, R is ready.
 */
constexpr std::array<sf::Keyboard::Scancode, BOUND_ACTION_COUNT> DEFAULT_KEYS{
    sf::Keyboard::Scan::Up,    sf::Keyboard::Scan::Down,
    sf::Keyboard::Scan::Left,  sf::Keyboard::Scan::Right,
    sf::Keyboard::Scan::Space, sf::Keyboard::Scan::R};

/** @brief A physical key and its name in capitals, as the options draw it
 * and the settings file writes it. */
struct NamedKey {
  sf::Keyboard::Scancode key{};
  std::string_view name;
};

/**
 * @brief Every key the player may bind, with its name: letters and digits as
 * on a US keyboard, the place of the key and not its character. A key outside
 * this list cannot be bound: Escape, which the menus keep, and the Command and
 * Windows keys, whose system shortcuts would leave the game, are left out on
 * purpose.
 */
constexpr std::array KEY_NAMES{
    NamedKey{.key = sf::Keyboard::Scan::A, .name = "A"},
    NamedKey{.key = sf::Keyboard::Scan::B, .name = "B"},
    NamedKey{.key = sf::Keyboard::Scan::C, .name = "C"},
    NamedKey{.key = sf::Keyboard::Scan::D, .name = "D"},
    NamedKey{.key = sf::Keyboard::Scan::E, .name = "E"},
    NamedKey{.key = sf::Keyboard::Scan::F, .name = "F"},
    NamedKey{.key = sf::Keyboard::Scan::G, .name = "G"},
    NamedKey{.key = sf::Keyboard::Scan::H, .name = "H"},
    NamedKey{.key = sf::Keyboard::Scan::I, .name = "I"},
    NamedKey{.key = sf::Keyboard::Scan::J, .name = "J"},
    NamedKey{.key = sf::Keyboard::Scan::K, .name = "K"},
    NamedKey{.key = sf::Keyboard::Scan::L, .name = "L"},
    NamedKey{.key = sf::Keyboard::Scan::M, .name = "M"},
    NamedKey{.key = sf::Keyboard::Scan::N, .name = "N"},
    NamedKey{.key = sf::Keyboard::Scan::O, .name = "O"},
    NamedKey{.key = sf::Keyboard::Scan::P, .name = "P"},
    NamedKey{.key = sf::Keyboard::Scan::Q, .name = "Q"},
    NamedKey{.key = sf::Keyboard::Scan::R, .name = "R"},
    NamedKey{.key = sf::Keyboard::Scan::S, .name = "S"},
    NamedKey{.key = sf::Keyboard::Scan::T, .name = "T"},
    NamedKey{.key = sf::Keyboard::Scan::U, .name = "U"},
    NamedKey{.key = sf::Keyboard::Scan::V, .name = "V"},
    NamedKey{.key = sf::Keyboard::Scan::W, .name = "W"},
    NamedKey{.key = sf::Keyboard::Scan::X, .name = "X"},
    NamedKey{.key = sf::Keyboard::Scan::Y, .name = "Y"},
    NamedKey{.key = sf::Keyboard::Scan::Z, .name = "Z"},
    NamedKey{.key = sf::Keyboard::Scan::Num1, .name = "1"},
    NamedKey{.key = sf::Keyboard::Scan::Num2, .name = "2"},
    NamedKey{.key = sf::Keyboard::Scan::Num3, .name = "3"},
    NamedKey{.key = sf::Keyboard::Scan::Num4, .name = "4"},
    NamedKey{.key = sf::Keyboard::Scan::Num5, .name = "5"},
    NamedKey{.key = sf::Keyboard::Scan::Num6, .name = "6"},
    NamedKey{.key = sf::Keyboard::Scan::Num7, .name = "7"},
    NamedKey{.key = sf::Keyboard::Scan::Num8, .name = "8"},
    NamedKey{.key = sf::Keyboard::Scan::Num9, .name = "9"},
    NamedKey{.key = sf::Keyboard::Scan::Num0, .name = "0"},
    NamedKey{.key = sf::Keyboard::Scan::Enter, .name = "ENTER"},
    NamedKey{.key = sf::Keyboard::Scan::Backspace, .name = "BACKSPACE"},
    NamedKey{.key = sf::Keyboard::Scan::Tab, .name = "TAB"},
    NamedKey{.key = sf::Keyboard::Scan::Space, .name = "SPACE"},
    NamedKey{.key = sf::Keyboard::Scan::Hyphen, .name = "-"},
    NamedKey{.key = sf::Keyboard::Scan::Equal, .name = "="},
    NamedKey{.key = sf::Keyboard::Scan::LBracket, .name = "["},
    NamedKey{.key = sf::Keyboard::Scan::RBracket, .name = "]"},
    NamedKey{.key = sf::Keyboard::Scan::Backslash, .name = "\\"},
    NamedKey{.key = sf::Keyboard::Scan::Semicolon, .name = ";"},
    NamedKey{.key = sf::Keyboard::Scan::Apostrophe, .name = "'"},
    NamedKey{.key = sf::Keyboard::Scan::Grave, .name = "`"},
    NamedKey{.key = sf::Keyboard::Scan::Comma, .name = ","},
    NamedKey{.key = sf::Keyboard::Scan::Period, .name = "."},
    NamedKey{.key = sf::Keyboard::Scan::Slash, .name = "/"},
    NamedKey{.key = sf::Keyboard::Scan::NonUsBackslash, .name = "<"},
    NamedKey{.key = sf::Keyboard::Scan::F1, .name = "F1"},
    NamedKey{.key = sf::Keyboard::Scan::F2, .name = "F2"},
    NamedKey{.key = sf::Keyboard::Scan::F3, .name = "F3"},
    NamedKey{.key = sf::Keyboard::Scan::F4, .name = "F4"},
    NamedKey{.key = sf::Keyboard::Scan::F5, .name = "F5"},
    NamedKey{.key = sf::Keyboard::Scan::F6, .name = "F6"},
    NamedKey{.key = sf::Keyboard::Scan::F7, .name = "F7"},
    NamedKey{.key = sf::Keyboard::Scan::F8, .name = "F8"},
    NamedKey{.key = sf::Keyboard::Scan::F9, .name = "F9"},
    NamedKey{.key = sf::Keyboard::Scan::F10, .name = "F10"},
    NamedKey{.key = sf::Keyboard::Scan::F11, .name = "F11"},
    NamedKey{.key = sf::Keyboard::Scan::F12, .name = "F12"},
    NamedKey{.key = sf::Keyboard::Scan::F13, .name = "F13"},
    NamedKey{.key = sf::Keyboard::Scan::F14, .name = "F14"},
    NamedKey{.key = sf::Keyboard::Scan::F15, .name = "F15"},
    NamedKey{.key = sf::Keyboard::Scan::F16, .name = "F16"},
    NamedKey{.key = sf::Keyboard::Scan::F17, .name = "F17"},
    NamedKey{.key = sf::Keyboard::Scan::F18, .name = "F18"},
    NamedKey{.key = sf::Keyboard::Scan::F19, .name = "F19"},
    NamedKey{.key = sf::Keyboard::Scan::CapsLock, .name = "CAPS LOCK"},
    NamedKey{.key = sf::Keyboard::Scan::Insert, .name = "INSERT"},
    NamedKey{.key = sf::Keyboard::Scan::Home, .name = "HOME"},
    NamedKey{.key = sf::Keyboard::Scan::PageUp, .name = "PAGE UP"},
    NamedKey{.key = sf::Keyboard::Scan::Delete, .name = "DELETE"},
    NamedKey{.key = sf::Keyboard::Scan::End, .name = "END"},
    NamedKey{.key = sf::Keyboard::Scan::PageDown, .name = "PAGE DOWN"},
    NamedKey{.key = sf::Keyboard::Scan::Right, .name = "RIGHT"},
    NamedKey{.key = sf::Keyboard::Scan::Left, .name = "LEFT"},
    NamedKey{.key = sf::Keyboard::Scan::Down, .name = "DOWN"},
    NamedKey{.key = sf::Keyboard::Scan::Up, .name = "UP"},
    NamedKey{.key = sf::Keyboard::Scan::NumpadDivide, .name = "PAD /"},
    NamedKey{.key = sf::Keyboard::Scan::NumpadMultiply, .name = "PAD *"},
    NamedKey{.key = sf::Keyboard::Scan::NumpadMinus, .name = "PAD -"},
    NamedKey{.key = sf::Keyboard::Scan::NumpadPlus, .name = "PAD +"},
    NamedKey{.key = sf::Keyboard::Scan::NumpadEqual, .name = "PAD ="},
    NamedKey{.key = sf::Keyboard::Scan::NumpadEnter, .name = "PAD ENTER"},
    NamedKey{.key = sf::Keyboard::Scan::NumpadDecimal, .name = "PAD ."},
    NamedKey{.key = sf::Keyboard::Scan::Numpad1, .name = "PAD 1"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad2, .name = "PAD 2"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad3, .name = "PAD 3"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad4, .name = "PAD 4"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad5, .name = "PAD 5"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad6, .name = "PAD 6"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad7, .name = "PAD 7"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad8, .name = "PAD 8"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad9, .name = "PAD 9"},
    NamedKey{.key = sf::Keyboard::Scan::Numpad0, .name = "PAD 0"},
    NamedKey{.key = sf::Keyboard::Scan::LControl, .name = "LEFT CTRL"},
    NamedKey{.key = sf::Keyboard::Scan::LShift, .name = "LEFT SHIFT"},
    NamedKey{.key = sf::Keyboard::Scan::LAlt, .name = "LEFT ALT"},
    NamedKey{.key = sf::Keyboard::Scan::RControl, .name = "RIGHT CTRL"},
    NamedKey{.key = sf::Keyboard::Scan::RShift, .name = "RIGHT SHIFT"},
    NamedKey{.key = sf::Keyboard::Scan::RAlt, .name = "RIGHT ALT"},
};

/** @brief First letter of the alphabet, the name of the first letter key. */
constexpr char FIRST_LETTER_NAME = 'A';

/** @brief What a keycap shows for a key with no name. */
constexpr std::string_view UNNAMED_KEY_TEXT = "?";

}  // namespace rtype::client
