#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include "PointerEvents.hpp"
#include "RecordingServerConnection.hpp"

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
constexpr sf::Vector2i BACK_CHIP_MIDDLE =
    sf::Vector2i(415, 238) * INTERFACE_PIXEL;

/** @brief The key READY is bound to in these tests, and the name its keycap
 * shows. */
constexpr sf::Keyboard::Scancode READY_KEY = sf::Keyboard::Scan::R;
constexpr const char* READY_KEY_NAME = "R";

/** @brief The two ready states a test expects the waiting room to ask. */
const ServerConnectionCall ASK_READY{
    .request = ServerConnectionCall::Request::SetReady,
    .server = {},
    .nickname = {},
    .ready = true};
const ServerConnectionCall ASK_NOT_READY{
    .request = ServerConnectionCall::Request::SetReady,
    .server = {},
    .nickname = {},
    .ready = false};

}  // namespace rtype::client::testing
