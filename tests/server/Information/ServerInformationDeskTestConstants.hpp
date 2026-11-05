#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "ServerOccupancy.hpp"

namespace rtype::server::testing {

/** @brief The name and the game version the desk of the tests answers. */
constexpr std::string_view DESK_SERVER_NAME = "Lyon";
constexpr std::string_view DESK_GAME_VERSION = "0.1.0";

/** @brief How full the server of the desk is: five players in two games,
 * three free seats. */
constexpr ServerOccupancy DESK_OCCUPANCY{
    .playerCount = 5, .gameCount = 2, .freeSeatCount = 3};

/** @brief The sentAt of the requests of the tests. */
constexpr std::uint32_t ASKED_AT = 0x01020304;

/** @brief When a request arrives, then three milliseconds later, when the
 * desk answers it: 3000 microseconds held. */
constexpr std::chrono::nanoseconds REQUEST_ARRIVAL{std::chrono::seconds{5}};
constexpr std::chrono::nanoseconds ANSWERED_THREE_MILLISECONDS_LATER =
    REQUEST_ARRIVAL + std::chrono::milliseconds{3};
constexpr std::uint32_t THREE_MILLISECONDS_IN_MICROSECONDS = 3000;

}  // namespace rtype::server::testing
