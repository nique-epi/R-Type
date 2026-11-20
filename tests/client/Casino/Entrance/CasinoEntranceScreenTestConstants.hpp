#pragma once

#include <chrono>
#include <string_view>

namespace rtype::client::testing {

/** @brief The pictures of the front of the casino. */
constexpr std::string_view DOORS_CLOSED_PICTURE =
    "sprites/casino/entrance_doors_closed.png";
constexpr std::string_view DOORS_OPEN_PICTURE =
    "sprites/casino/entrance_doors_open.png";

/** @brief What the butler says first before the doors. */
constexpr std::string_view WELCOME_FIRST_LINE = "WELCOME TO THE BYDO PALACE,";

/** @brief How long the doors take to open: the greeting and the bow of the
 * butler, four frames of 150 ms each, 1.2 s; a time short of it, a time
 * inside the greeting, while the doors stay closed, and a time inside the
 * bow, once they opened. Each counts from the frame of 16 ms that followed
 * the release of ENTER: 16 ms then 1100 ms make 1116 ms, short of 1.2 s. */
constexpr std::chrono::milliseconds DOORS_OPENING_TIME{1200};
constexpr std::chrono::milliseconds JUST_BEFORE_THE_HALL{1100};
constexpr std::chrono::milliseconds DURING_THE_GREETING{300};
constexpr std::chrono::milliseconds DURING_THE_BOW{900};

}  // namespace rtype::client::testing
