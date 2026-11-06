#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/** @brief Nicknames of the players of a game, and one that is too long: 17
 * bytes, one more than a nickname holds. */
constexpr std::string_view ACE = "ACE";
constexpr std::string_view BOB = "BOB";
constexpr std::string_view CID = "CID";
constexpr std::string_view DAN = "DAN";
constexpr std::string_view EVE = "EVE";
constexpr std::string_view NICKNAME_OF_SEVENTEEN_BYTES = "ABCDEFGHIJKLMNOPQ";

/** @brief A measure that fills its four bytes, 0x01020304 microseconds, and
 * two other values. */
constexpr std::uint32_t FOUR_BYTE_MEASURE = 0x01020304;
constexpr std::uint32_t TICK_MEDIAN_MICROSECONDS = 2000;
constexpr std::uint32_t AVERAGE_ROUND_TRIP_MICROSECONDS = 30000;

/** @brief A measures byte whose bit 3 is set, which no measure uses. */
constexpr unsigned int UNKNOWN_MEASURE_BIT_BYTE = 0x08;

/** @brief A count of five nicknames, one more than a game holds. */
constexpr unsigned int FIVE_NICKNAMES_BYTE = 5;

}  // namespace rtype::network::testing
