#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <string_view>
#include "PointerEvents.hpp"

namespace rtype::client::testing {

/** @brief The local player, and another one in the third bay. */
constexpr std::uint8_t OWN_NUMBER = 1;
constexpr std::uint8_t OTHER_NUMBER = 3;
constexpr const char* OWN_NICKNAME = "ACE";
constexpr const char* OTHER_NICKNAME = "CAPTAIN_STARDUST";

/**
 * @brief Where the waiting room the team chose writes things: the nicknames
 * of the first and third bays on their middle columns, row 120; YOU at the
 * end of the number row of the first bay; the inside of the cell of the
 * first bay, 8 in and 86 down less its border; the middle of the BACK chip, in
 * pixels of the window.
 */
constexpr sf::Vector2i FIRST_BAY_NICKNAME(75, 120);
constexpr sf::Vector2i THIRD_BAY_NICKNAME(295, 120);
constexpr sf::Vector2i FIRST_BAY_LOCAL_TAG(118, 58);
constexpr sf::IntRect FIRST_BAY_CELL_INSIDE({33, 137}, {84, 14});
/** @brief The middle of the bay of the local player, the first one (24 to
 * 126 across, 50 to 162 down), and of the third bay, 220 pixels to its right,
 * in pixels of the window; what the head holds for the top hat. */
constexpr sf::Vector2i OWN_BAY_MIDDLE = sf::Vector2i(75, 100) * INTERFACE_PIXEL;
constexpr sf::Vector2i THIRD_BAY_MIDDLE =
    sf::Vector2i(295, 100) * INTERFACE_PIXEL;
constexpr std::uint8_t WEARS_TOP_HAT = 1;
constexpr const char* LOOK_SHEET = "sprites/looks.png";
constexpr sf::Vector2i BACK_CHIP_MIDDLE =
    sf::Vector2i(415, 238) * INTERFACE_PIXEL;

/** @brief The middle of RANKING: BACK starts at 374, HELP at 300, and RANKING,
 * 90 wide, 8 to its left. */
constexpr sf::Vector2i LEADERBOARD_CHIP_MIDDLE =
    sf::Vector2i(247, 238) * INTERFACE_PIXEL;

/** @brief The key READY is bound to in these tests: the key of a first
 * launch, DEFAULT_KEYS, since the fixture keeps the bindings of a first
 * launch. */
constexpr sf::Keyboard::Scancode READY_KEY = sf::Keyboard::Scan::R;

/** @brief The two ready states a test expects the waiting room to ask. */
constexpr bool ASK_READY = true;
constexpr bool ASK_NOT_READY = false;

/** @brief The key of a first launch that opens the chat, a letter typed into
 * it, and the line it makes typed twice. */
constexpr sf::Keyboard::Scancode CHAT_KEY = sf::Keyboard::Scan::T;
constexpr char32_t CHAT_LETTER = U'g';
constexpr std::string_view CHAT_LINE = "gg";

}  // namespace rtype::client::testing
