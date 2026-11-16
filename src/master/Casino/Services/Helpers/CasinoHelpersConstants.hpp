#pragma once

#include <array>
#include <string_view>

namespace rtype::master {

/** @brief The names of the first rooms of the casino, from
 * network::FIRST_CASINO_ROOM on; a room past the list is named
 * UNNAMED_ROOM_PREFIX and its number. Each fits
 * network::MAXIMUM_CASINO_ROOM_NAME_SIZE. */
constexpr auto CASINO_ROOM_NAMES = std::to_array<std::string_view>(
    {"Bydo Palace", "Nebula Lounge", "Wave Cannon Club", "Force Pod Parlour",
     "Dobkeratops Den", "Asteroid Belt Bar", "R-9 Officers Mess",
     "Last Dance Hall"});
constexpr std::string_view UNNAMED_ROOM_PREFIX = "Room ";

/** @brief How the journal of a wallet names a round of the casino, in the
 * stakes, refunds and payouts it moved: `round:<room>:<round>`. */
constexpr std::string_view ROUND_REFERENCE_PREFIX = "round:";
constexpr std::string_view ROUND_REFERENCE_SEPARATOR = ":";

}  // namespace rtype::master
