#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/**
 * @brief Lines that set every rule to a value other than its default: the
 * difficulty "hard", five lives and friendly fire on.
 */
constexpr std::string_view SET_HARD_DIFFICULTY_LINE = "set difficulty hard";
constexpr std::string_view SET_FIVE_LIVES_LINE = "set lives 5";
constexpr std::string_view SET_FRIENDLY_FIRE_ON_LINE = "set friendly-fire 1";
constexpr std::string_view HARD_DIFFICULTY_NAME = "hard";
constexpr std::uint8_t FIVE_LIVES_COUNT = 5;

/**
 * @brief Lines at the bounds of the lives a game accepts, 1 to 9: no life, the
 * most, one past the most, and 257, which a cut to 8 bits would read as 1.
 */
constexpr std::string_view SET_NO_LIFE_LINE = "set lives 0";
constexpr std::string_view SET_NINE_LIVES_LINE = "set lives 9";
constexpr std::string_view SET_TEN_LIVES_LINE = "set lives 10";
constexpr std::string_view SET_LIVES_PAST_EIGHT_BITS_LINE = "set lives 257";
constexpr std::uint8_t NINE_LIVES_COUNT = 9;

/** @brief A friendly fire just above 1, the value that turns it on. */
constexpr std::string_view SET_FRIENDLY_FIRE_TWO_LINE = "set friendly-fire 2";

}  // namespace rtype::server::testing
