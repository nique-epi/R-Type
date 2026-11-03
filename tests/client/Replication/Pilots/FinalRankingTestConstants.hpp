#pragma once

#include <cstdint>
#include <limits>
#include <string_view>

namespace rtype::client::testing {

/** @brief A board of three pilots, the second being this client, and the
 * scores of a game. */
constexpr std::uint8_t FIRST_NUMBER = 1;
constexpr std::uint8_t SECOND_NUMBER = 2;
constexpr std::uint8_t THIRD_NUMBER = 3;
constexpr std::uint8_t UNLISTED_NUMBER = 4;
constexpr std::string_view FIRST_NICKNAME = "ACE";
constexpr std::string_view SECOND_NICKNAME = "NOVA";
constexpr std::string_view THIRD_NICKNAME = "KAI";
constexpr std::uint32_t LOW_SCORE = 100;
constexpr std::uint32_t HIGH_SCORE = 300;
constexpr std::uint32_t MIDDLE_SCORE = 200;
constexpr std::uint32_t LOW_PLUS_HIGH_PLUS_MIDDLE = 600;
constexpr std::uint32_t LARGEST_UNSIGNED =
    std::numeric_limits<std::uint32_t>::max();
constexpr int FIRST_RANK = 1;
constexpr int THIRD_RANK = 3;

}  // namespace rtype::client::testing
