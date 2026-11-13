#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief Moments of the shop: a quarter of a breath (0.75 s), where the
 * shopkeeper is at his lowest, 2 pixels under his rest; three quarters
 * (2.25 s), at his highest, 2 pixels above; a whole breath (3 s). */
constexpr engine::Duration A_QUARTER_OF_A_BREATH =
    std::chrono::milliseconds(750);
constexpr engine::Duration THREE_QUARTERS_OF_A_BREATH =
    std::chrono::milliseconds(2250);
constexpr engine::Duration A_WHOLE_BREATH = std::chrono::seconds(3);
constexpr int LOWEST_DROP = 2;
constexpr int HIGHEST_DROP = -2;

/** @brief Moments of the eye: the middle of its first blink (1.875 s), of
 * its glance to the left (3.5 s) and to the right (5.75 s), and a whole
 * cycle (8 s). */
constexpr engine::Duration THE_FIRST_BLINK = std::chrono::milliseconds(1875);
constexpr engine::Duration THE_GLANCE_LEFT = std::chrono::milliseconds(3500);
constexpr engine::Duration THE_GLANCE_RIGHT = std::chrono::milliseconds(5750);
constexpr engine::Duration A_WHOLE_EYE_CYCLE = std::chrono::seconds(8);

}  // namespace rtype::client::testing
