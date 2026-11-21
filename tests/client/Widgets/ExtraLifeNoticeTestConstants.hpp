#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief The second player, its bay, the second from the left, and the
 * column its notice starts at: the bay starts at 1 + 1 × (118 + 2), and its
 * lives 5 columns in. */
constexpr std::uint8_t SECOND_PLAYER = 2;
constexpr std::size_t SECOND_BAY = 1;
constexpr int SECOND_BAY_LIVES_COLUMN = 126;

/** @brief When the life was won, since the start of the game, and the time
 * the notice lasts: 1.5 s, over which it rises 6 rows. */
constexpr std::chrono::nanoseconds LIFE_WON_AT = std::chrono::seconds(40);
constexpr std::chrono::nanoseconds EXTRA_LIFE_SHOWN =
    std::chrono::milliseconds(1500);
constexpr std::chrono::nanoseconds HALF_THE_NOTICE =
    std::chrono::milliseconds(750);
constexpr int NOT_RISEN = 0;
constexpr int HALF_RISEN = 3;

constexpr std::string_view EXTRA_LIFE_WORD = "1UP";
