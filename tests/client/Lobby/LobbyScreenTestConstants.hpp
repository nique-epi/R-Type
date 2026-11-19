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

/** @brief The words every rules line holds, and the row the line stands on:
 * the heading at SAFE_MARGIN 24, its baseline 8 down, the baseline of the
 * small font 7 down, so 25. */
constexpr std::string_view RULES_FRIENDLY_FIRE_WORDS = "FRIENDLY FIRE";
constexpr int RULES_LINE_TOP = 25;
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

/**
 * @brief The maps of the room in the tests of the vote, and their numbers
 * from 1; what the MAP bar says of each, and of a room without maps; what a
 * bay writes of a vote for the second; the key of MAP; the middle of the MAP
 * bar, which starts at x 244 (READY at 76, 160 wide, then BUTTON_GAP) and
 * y 174, 160 by 20, in pixels of the window.
 */
constexpr const char* FIRST_VOTED_MAP = "OPEN SPACE";
constexpr const char* SECOND_VOTED_MAP = "THE PIT";
constexpr std::uint8_t FIRST_MAP_NUMBER = 1;
constexpr std::uint8_t SECOND_MAP_NUMBER = 2;
constexpr const char* FIRST_MAP_BAR = "MAP: OPEN SPACE";
constexpr const char* SECOND_MAP_BAR = "MAP: THE PIT";
constexpr const char* NO_MAP_BAR = "MAP: FIXED";
constexpr const char* SECOND_MAP_VOTE_LINE = "VOTE: THE PIT";
constexpr sf::Keyboard::Key MAP_KEY_CODE = sf::Keyboard::Key::M;
/** @brief Where the M of an AZERTY keyboard lies: the place of the semicolon
 * of a QWERTY one, so a test of MAP presses the M of a layout where the place
 * and the letter differ. */
constexpr sf::Keyboard::Scancode AZERTY_M_PLACE = sf::Keyboard::Scan::Semicolon;
constexpr sf::Vector2i MAP_BAR_MIDDLE =
    sf::Vector2i(324, 184) * INTERFACE_PIXEL;

/**
 * @brief The previews of the two maps, their path in the pack; what the card
 * says while nobody voted; a point
 * of the card left of its preview, 5 pixels into the card that starts where
 * the MAP bar does (324 less half of its 160); and the size of the pip of a
 * vote.
 */
constexpr const char* FIRST_MAP_PREVIEW = "previews/open_space.png";
constexpr const char* SECOND_MAP_PREVIEW = "previews/the_pit.png";
constexpr const char* NO_MAP_CHOSEN = "RANDOM";
constexpr sf::Vector2i LEFT_OF_THE_PREVIEW =
    sf::Vector2i(249, 184) * INTERFACE_PIXEL;
constexpr sf::Vector2i VOTE_PIP = sf::Vector2i(4, 4);

}  // namespace rtype::client::testing
