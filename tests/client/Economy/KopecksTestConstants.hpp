#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The largest balance. */
constexpr std::uint32_t LARGEST_BALANCE = 999'999;
constexpr std::uint32_t FOUR_DIGIT_AMOUNT = 1250;
constexpr std::string_view FOUR_DIGITS_WRITTEN = "1 250";
constexpr std::uint32_t THREE_DIGIT_AMOUNT = 999;
constexpr std::string_view THREE_DIGITS_WRITTEN = "999";
constexpr std::string_view LARGEST_BALANCE_WRITTEN = "999 999";

}  // namespace rtype::client::testing
