#pragma once

#include <cstdint>
#include <string_view>
#include "Purse.hpp"
#include "SettingsConstants.hpp"

namespace rtype::client::testing {

/** @brief A purse of 4 250 kopecks and 1 250 chips, and amounts that the
 * cashier refuses: none, 250 chips more than held, and a purse whose chips
 * are full, so buying one more is refused. */
constexpr Purse A_PURSE{.kopecks = 4'250, .chips = 1'250, .locker = {}};
constexpr std::uint32_t NO_AMOUNT = 0;
constexpr std::uint32_t TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY = 1'500;
constexpr Purse FULL_CHIPS{
    .kopecks = 4'250, .chips = rtype::client::MAXIMUM_CHIPS, .locker = {}};
constexpr std::uint32_t ONE_CHIP = 1;

/** @brief What the cashier says of each. */
constexpr std::string_view CHOOSE_AN_AMOUNT = "CHOOSE AN AMOUNT";
constexpr std::string_view TWO_HUNDRED_FIFTY_CHIPS_MISSING =
    "NOT ENOUGH CHIPS: 250 MISSING";
constexpr std::string_view NO_ROOM_FOR_MORE_CHIPS =
    "NO ROOM LEFT FOR MORE CHIPS";

}  // namespace rtype::client::testing
