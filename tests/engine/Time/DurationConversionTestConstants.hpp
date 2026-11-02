#pragma once

#include <chrono>
#include <cstddef>

/** @brief A duration with a fraction of a millisecond, and the same duration as
 * a number of milliseconds. */
constexpr std::chrono::microseconds ONE_AND_A_HALF_MILLISECONDS{1500};
constexpr double ONE_AND_A_HALF = 1.5;

/** @brief A total, how many parts it is shared between, and the share of each.
 */
constexpr std::chrono::milliseconds TOTAL_OF_THREE_PARTS{6};
constexpr std::size_t THREE_PARTS = 3;
constexpr std::chrono::milliseconds SHARE_OF_ONE_PART{2};
