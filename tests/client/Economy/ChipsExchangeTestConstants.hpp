#pragma once

#include <cstdint>
#include "KopecksTestConstants.hpp"

namespace rtype::client::testing {

/** @brief A purse of the cashier pictures: 4 250 kopecks and 1 250 chips, and
 * an exchange of 500, one for one: 3 750 kopecks and 1 750 chips once bought,
 * 4 750 kopecks and 750 chips once cashed out. */
constexpr std::uint32_t SOME_KOPECKS = 4'250;
constexpr std::uint32_t SOME_CHIPS = 1'250;
constexpr std::uint32_t AN_AMOUNT = 500;
constexpr std::uint32_t KOPECKS_AFTER_BUYING = 3'750;
constexpr std::uint32_t CHIPS_AFTER_BUYING = 1'750;
constexpr std::uint32_t KOPECKS_AFTER_CASHING_OUT = 4'750;
constexpr std::uint32_t CHIPS_AFTER_CASHING_OUT = 750;

/** @brief A balance that an exchange of AN_AMOUNT fills exactly up to
 * LARGEST_BALANCE (KopecksTestConstants.hpp), the largest number of kopecks
 * and of chips the game keeps. */
constexpr std::uint32_t AN_AMOUNT_SHORT_OF_THE_LARGEST =
    LARGEST_BALANCE - AN_AMOUNT;

}  // namespace rtype::client::testing
