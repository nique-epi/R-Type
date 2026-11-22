#pragma once

#include <cstdint>
#include <string_view>
#include "Wallet.hpp"

namespace rtype::client::testing {

/** @brief A wallet of 4 250 kopecks and 1 250 chips, and amounts that the
 * cashier refuses: none, 250 chips or kopecks more than held, and a wallet
 * whose chips are full, so buying one more is refused. */
constexpr network::Wallet A_WALLET{.kopecks = 4'250, .chips = 1'250};
constexpr std::uint32_t NO_AMOUNT = 0;
constexpr std::uint32_t TWO_HUNDRED_FIFTY_CHIPS_TOO_MANY = 1'500;
constexpr std::uint32_t TWO_HUNDRED_FIFTY_KOPECKS_TOO_MANY = 4'500;
constexpr network::Wallet FULL_CHIPS{.kopecks = 4'250, .chips = 999'999};
constexpr std::uint32_t ONE_CHIP = 1;

/** @brief What the cashier says of each. */
constexpr std::string_view CHOOSE_AN_AMOUNT = "CHOOSE AN AMOUNT";
constexpr std::string_view TWO_HUNDRED_FIFTY_CHIPS_MISSING =
    "NOT ENOUGH CHIPS: 250 MISSING";
constexpr std::string_view TWO_HUNDRED_FIFTY_KOPECKS_MISSING =
    "NOT ENOUGH KOPECKS: 250 MISSING";
constexpr std::string_view FRENCH_TWO_HUNDRED_FIFTY_KOPECKS_MISSING =
    "KOPECKS INSUFFISANTS : IL EN MANQUE 250";
constexpr std::string_view NO_ROOM_FOR_MORE_CHIPS =
    "NO ROOM LEFT FOR MORE CHIPS";

}  // namespace rtype::client::testing
