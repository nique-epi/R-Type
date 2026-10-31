#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief Pilots of the feed and its rules as the design sets them: a line
 * kept five seconds, three lines at most, a name cut to ten letters; after
 * pilots 1 to 4 left, the oldest line kept is that of pilot 2. */
constexpr std::uint8_t OLDEST_KEPT_NUMBER = 2;
constexpr std::uint8_t NOVA_NUMBER = 2;
constexpr std::uint8_t KAI_NUMBER = 3;
constexpr std::string_view NOVA = "NOVA";
constexpr std::string_view KAI = "KAI";
constexpr std::string_view LONG_NAME = "CAPTAIN_STARDUST";
constexpr std::string_view LONG_NAME_CUT = "CAPTAIN_ST";
constexpr std::string_view FEED_LEFT_WORDS = "LEFT THE GAME";
constexpr std::string_view FEED_LOST_WORDS = "SIGNAL LOST";
constexpr std::size_t MOST_LINES = 3;
constexpr rtype::engine::Duration LINE_TIME = std::chrono::seconds(5);

/** @brief The first line as the design measures it: the line starts at
 * (4, 5); the name is written 11 pixels to the right and 2 rows lower, at
 * (15, 7); the plug, 8 pixels square, ends at the right of the 9-pixel icon
 * column and starts on the row of the name, at (5, 7). */
constexpr sf::Vector2i FIRST_NAME_ANCHOR(15, 7);
constexpr sf::IntRect FIRST_PLUG_AREA({5, 7}, {8, 8});
