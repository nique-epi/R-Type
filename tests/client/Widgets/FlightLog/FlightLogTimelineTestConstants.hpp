#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The game of the design system takes input at 7.75 s, and writes a
 * row every 0.4 s (card "A v2", "Timeline"); a game of four pilots. */
constexpr rtype::engine::Duration DESIGN_READY =
    std::chrono::milliseconds(7750);
constexpr rtype::engine::Duration DESIGN_ROW_STEP =
    std::chrono::milliseconds(400);
constexpr std::size_t FOUR_PILOTS = 4;

}  // namespace rtype::client::testing
