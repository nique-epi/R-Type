#pragma once

#include <cstdint>
#include <limits>
#include <string_view>

namespace rtype::master {

/** @brief What the answers of the routes of the casino say when they
 * refuse. */
constexpr std::string_view UNKNOWN_ROOM_DETAIL = "the casino has no such room";
constexpr std::string_view UNKNOWN_ROUND_DETAIL =
    "a round is a whole number from 0";
constexpr std::string_view ROOM_FULL_DETAIL = "every seat of the room is taken";
constexpr std::string_view BETS_CLOSED_DETAIL =
    "the bets of that round are closed";
constexpr std::string_view NOT_SEATED_DETAIL =
    "take a seat in the room before betting";
constexpr std::string_view INVALID_BET_DETAIL =
    "a bet lies off the cloth or holds no chip";
constexpr std::string_view TOO_MANY_BETS_DETAIL =
    "a round takes 32 bets at most";
constexpr std::string_view OVER_THE_CAP_DETAIL =
    "the bets of a round hold 5000 chips at most";
constexpr std::string_view NOT_ENOUGH_CHIPS_DETAIL =
    "the bets hold more chips than the wallet";

/** @brief The rounds a path may name: any whole number of 64 signed bits from
 * 0. */
constexpr std::int64_t FIRST_ROUND = 0;
constexpr std::int64_t LAST_ROUND = std::numeric_limits<std::int64_t>::max();

}  // namespace rtype::master
