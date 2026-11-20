#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include "PointerEvents.hpp"

namespace rtype::client::testing {

/** @brief The rooms the master lists: the first full, the second with a
 * seat free, both of HALL_ROOM_SEATS seats. */
constexpr std::uint8_t HALL_FULL_ROOM = 1;
constexpr std::uint8_t HALL_FREE_ROOM = 2;
constexpr std::uint8_t HALL_ROOM_SEATS = 8;

/** @brief How often the hall asks the rooms again: every 5 s. */
constexpr std::chrono::seconds HALL_ROOMS_ASKED_AGAIN_AFTER{5};

/** @brief The hall fades for 360 ms before it opens a room: one frame of
 * 16 ms then 300 ms make 316 ms, short of it. */
constexpr std::chrono::milliseconds THE_WHOLE_FADE{360};
constexpr std::chrono::milliseconds MOST_OF_THE_FADE{300};

/** @brief Pixels of the window over the middle of each arch of the hall
 * (left arch 0 to 29 by 40 to 149, right arch 406 to 465 by 62 to 141), and
 * one over neither, each pixel of the picture INTERFACE_PIXEL of the
 * window. */
constexpr sf::Vector2i OVER_THE_LEFT_ARCH =
    sf::Vector2i(15, 95) * INTERFACE_PIXEL;
constexpr sf::Vector2i OVER_THE_RIGHT_ARCH =
    sf::Vector2i(436, 102) * INTERFACE_PIXEL;
constexpr sf::Vector2i OVER_THE_RING = sf::Vector2i(240, 120) * INTERFACE_PIXEL;

/** @brief The title the hall shows on its cartouche. */
constexpr const char* MAIN_HALL_TITLE = "MAIN HALL";

}  // namespace rtype::client::testing
