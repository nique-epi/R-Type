#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::client::testing {

/** @brief A taunt line holds 18 characters at most (card "A v2"). */
constexpr std::size_t LABEL_LINE_LETTERS = 18;

/** @brief "Une vingtaine de phrases"; at least two taunts fit any game; the
 * seeds a draw is tried with. */
constexpr std::size_t ABOUT_TWENTY = 20;
constexpr std::size_t UNCONDITIONAL_AT_LEAST = 2;
constexpr std::uint32_t SEEDS_TRIED = 200;

/** @brief Scores of the pilots of a test, from the best (BOTTOM_SCORE between
 * SECOND_SCORE and SMALL_SCORE). */
constexpr std::uint32_t TOP_SCORE = 300;
constexpr std::uint32_t SECOND_SCORE = 200;
constexpr std::uint32_t SMALL_SCORE = 50;
constexpr std::uint32_t ZERO_SCORE = 0;

}  // namespace rtype::client::testing
