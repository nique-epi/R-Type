#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief A score of 12450 pays 1245 kopecks; one of 9 pays none. */
constexpr std::uint32_t DESIGN_SCORE = 12450;
constexpr std::uint32_t KOPECKS_OF_THE_DESIGN_SCORE = 1245;
constexpr std::uint32_t SCORE_UNDER_ONE_KOPECK = 9;
/** @brief A quarter more: 1000 gives 1250, and 30 gives 37, 7.5 rounded
 * down. */
constexpr std::uint32_t ROUND_GAIN = 1000;
constexpr std::uint32_t ROUND_GAIN_AFTER_A_VICTORY = 1250;
constexpr std::uint32_t ODD_GAIN = 30;
constexpr std::uint32_t ODD_GAIN_AFTER_A_VICTORY = 37;
/** @brief The largest balance, and the largest score of the protocol. */
constexpr std::uint32_t LARGEST_BALANCE = 999'999;
constexpr std::uint32_t LARGEST_SCORE = 4'294'967'295;
constexpr std::uint32_t FOUR_DIGIT_AMOUNT = 1250;
constexpr std::string_view FOUR_DIGITS_WRITTEN = "1 250";
constexpr std::uint32_t THREE_DIGIT_AMOUNT = 999;
constexpr std::string_view THREE_DIGITS_WRITTEN = "999";
constexpr std::string_view LARGEST_BALANCE_WRITTEN = "999 999";

}  // namespace rtype::client::testing
