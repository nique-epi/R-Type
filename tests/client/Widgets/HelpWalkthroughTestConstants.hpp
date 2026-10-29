#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include "LobbyPartAreas.hpp"
#include "PointerEvents.hpp"

namespace rtype::client::testing {

/** @brief The parts of the room the help lights, where LobbyScreen paints
 * them with the local player in the first bay; and its HELP chip, BUTTON_GAP
 * left of BACK. */
constexpr LobbyPartAreas ROOM_AREAS{.bays = {{24, 50}, {432, 112}},
                                    .ownBay = {{24, 50}, {102, 112}},
                                    .ready = {{160, 174}, {160, 20}},
                                    .status = {{160, 202}, {160, 8}},
                                    .back = {{374, 230}, {82, 16}}};
constexpr sf::IntRect HELP_CHIP({300, 230}, {66, 16});

/** @brief Pixels of the window, under the middle of the chip and in the room
 * left of it. */
constexpr sf::Vector2i HELP_CHIP_MIDDLE =
    sf::Vector2i(333, 238) * INTERFACE_PIXEL;
constexpr sf::Vector2i BESIDE_HELP_CHIP =
    sf::Vector2i(290, 238) * INTERFACE_PIXEL;

/** @brief Indices of steps, from 0: the third shows the bay of the local
 * player, the fourth READY, the sixth BACK, the seventh is the last of the
 * room, the eighth the first of the game, the ninth shows the moves, the
 * eleventh the Bydos, the fifteenth is the last. */
constexpr std::size_t OWN_BAY_STEP = 2;
constexpr std::size_t READY_STEP = 3;
constexpr std::size_t BACK_STEP = 5;
constexpr std::size_t LAST_ROOM_STEP = 6;
constexpr std::size_t MOVES_STEP = 8;
constexpr std::size_t BYDOS_STEP = 10;
constexpr std::size_t LAST_STEP = 14;

/** @brief First word of the first, second and last steps, and of the steps
 * that explain READY and the bay of the local player. */
constexpr const char* FIRST_STEP_WORD = "HELLO,";
constexpr const char* SECOND_STEP_WORD = "EACH";
constexpr const char* LAST_STEP_WORD = "THAT";
constexpr const char* READY_STEP_WORD = "PRESS";
constexpr const char* OWN_BAY_STEP_WORD = "THIS";

/** @brief The text of the chip, and the key bound to move up in a test. */
constexpr const char* CHIP_LABEL = "HELP";
constexpr const char* W_KEY_NAME = "W";

/** @brief Edges of the text of the bubble: 8 pixels in from a bubble 304
 * pixels wide centered on the 480 pixels of the picture. */
constexpr int BUBBLE_TEXT_LEFT = 96;
constexpr int BUBBLE_TEXT_RIGHT = 384;

}  // namespace rtype::client::testing
