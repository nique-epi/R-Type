#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The least time between two choices of look the relay takes from one
 * player. Its client sends a choice again every 500 ms until it is shown, so a
 * choice ignored is taken at the next copy.
 */
constexpr engine::Duration LOOK_CHOICE_INTERVAL =
    std::chrono::milliseconds(100);

/**
 * @brief LOOK_CHOICE_INTERVAL, in whole simulation ticks: 6.
 */
constexpr std::size_t TICKS_BETWEEN_LOOK_CHOICES =
    engine::wholeTicksIn(LOOK_CHOICE_INTERVAL);

}  // namespace rtype::server
