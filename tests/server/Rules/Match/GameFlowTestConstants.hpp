#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/** @brief The ticks the game needs after the last Bydo is gone to find it. */
constexpr std::size_t TICKS_TO_FIND_THE_SKY_CLEAR = 1;

/** @brief A second of ticks: the Bydos of the wave are still far from the
 * ships. */
constexpr std::size_t A_SECOND_OF_TICKS =
    engine::wholeTicksIn(std::chrono::seconds(1));

}  // namespace rtype::server::testing
