#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::client::testing {

/** @brief The places of the steps of 1 and of 500 among the chips of the
 * selector, and a place past the last. */
constexpr std::size_t STEP_OF_ONE_PLACE = 0;
constexpr std::size_t STEP_OF_FIVE_HUNDRED_PLACE = 4;
constexpr std::size_t PAST_THE_LAST_STEP = 5;

/** @brief The amount the cashier opens on, and its step: 500 by 100. */
constexpr std::uint32_t OPENING_AMOUNT = 500;
constexpr std::size_t OPENING_STEP_PLACE = 3;

/** @brief More steps of 500 than the largest balance holds: 999 999 / 500 is
 * 1 999 and some. */
constexpr int MORE_STEPS_THAN_THE_LARGEST_BALANCE = 2'100;

}  // namespace rtype::client::testing
