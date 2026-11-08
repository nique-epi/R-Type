#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The names the dialog shows for two tables of a waves file. */
constexpr std::string_view NORMAL_SHOWN = "NORMAL";
constexpr std::string_view EASY_SHOWN = "EASY";

/** @brief The fewest and the most lives a creator may give, as the server
 * accepts them, and the lives of a game nobody changed. */
constexpr unsigned int LIVES_AT_THE_FEWEST = 1;
constexpr unsigned int LIVES_AT_THE_MOST = 9;
constexpr unsigned int UNTOUCHED_LIVES = 3;

/** @brief Down presses from the first row to CREATE, past the three rows and
 * BACK, and from the lives row to CREATE. */
constexpr int ROWS_DOWN_TO_CREATE = 4;
constexpr int ROWS_FROM_LIVES_TO_CREATE = 3;

/** @brief The server whose games the map listed. */
constexpr std::string_view LISTED_SERVER_ADDRESS = "203.0.113.8";
constexpr std::uint16_t LISTED_SERVER_PORT = 4242;

/** @brief Presses that walk past the bounds of the lives: more than the
 * eight steps between one and nine. */
constexpr int PAST_THE_BOUNDS = 9;

}  // namespace rtype::client::testing
