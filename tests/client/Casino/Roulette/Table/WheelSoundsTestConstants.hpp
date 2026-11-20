#pragma once

#include <chrono>

namespace rtype::client::testing {

/**
 * @brief Times left of the spin of 6 s of the rooms of the tests, on either
 * side of the drop of the ball, which falls in its pocket once the rotor has
 * BALL_DROP_FRAMES (8) frames left, 1.36 s before the end (4 frames of
 * 200 ms then 4 of 140 ms): 2 s left is 13 frames, the ball still on its
 * track; 1 s left is 6 frames, the ball in its pocket.
 */
constexpr std::chrono::seconds TWO_SECONDS_BEFORE_THE_RESULT{2};
constexpr std::chrono::seconds ONE_SECOND_BEFORE_THE_RESULT{1};

}  // namespace rtype::client::testing
