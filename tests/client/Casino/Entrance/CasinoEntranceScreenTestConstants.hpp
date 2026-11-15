#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief What the player holds in these tests, and how the balances of the
 * hall write it: thousands apart. */
constexpr std::uint32_t KOPECKS_HELD = 4250;
constexpr std::uint32_t CHIPS_HELD = 1250;
constexpr std::string_view KOPECKS_HELD_TEXT = "4 250";
constexpr std::string_view CHIPS_HELD_TEXT = "1 250";

/** @brief The pictures of the front of the casino. */
constexpr std::string_view DOORS_CLOSED_PICTURE =
    "sprites/casino/entrance_doors_closed.png";
constexpr std::string_view DOORS_AJAR_PICTURE =
    "sprites/casino/entrance_doors_ajar.png";
constexpr std::string_view DOORS_OPEN_PICTURE =
    "sprites/casino/entrance_doors_open.png";

/** @brief What the butler says first before the doors, and in the hall. */
constexpr std::string_view WELCOME_FIRST_LINE =
    "WELCOME TO THE BYDO PALACE, PILOT.";
constexpr std::string_view HALL_FIRST_LINE = "WHAT SHALL IT BE TONIGHT, PILOT?";

/** @brief The label of the first bar of the hall. */
constexpr std::string_view ROULETTE_WORD = "ROULETTE";

/** @brief How long the doors take to open: the greeting and the bow of the
 * butler, four frames of 150 ms each, 1.2 s; a time short of it, and a time
 * inside the greeting, while the doors stand ajar. */
constexpr std::chrono::milliseconds DOORS_OPENING_TIME{1200};
constexpr std::chrono::milliseconds JUST_BEFORE_THE_HALL{1100};
constexpr std::chrono::milliseconds DURING_THE_GREETING{300};

}  // namespace rtype::client::testing
