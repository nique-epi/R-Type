#pragma once

#include <chrono>

namespace rtype::client {

/** @brief How long an arm takes to fetch an item and fit it. */
constexpr std::chrono::milliseconds FITTING_DURATION{3900};

/** @brief Where each phase of a fitting ends, as a share of
 * FITTING_DURATION: the jaws let go of the hull, the arm reaches off screen,
 * closes on the item, comes back above the mount, sets the item down, lets
 * go, goes back, and closes on the hull again. The old item of the slot goes
 * at FITTING_HOVER_END, the new one is on the ship from FITTING_PLACE_AT. */
constexpr float FITTING_RELEASE_END = 0.08F;
constexpr float FITTING_REACH_OUT_END = 0.30F;
constexpr float FITTING_GRAB_AT = 0.36F;
constexpr float FITTING_LEAVE_END = 0.40F;
constexpr float FITTING_HOVER_END = 0.62F;
constexpr float FITTING_PLACE_AT = 0.72F;
constexpr float FITTING_LET_GO_END = 0.78F;
constexpr float FITTING_BACK_END = 0.92F;

/** @brief How far the jaws open, 0 to 1, while they hold nothing; how far
 * above (or under) its mount an item is carried before it is set down; how
 * long the sparks of a placed or removed item last, as a share. */
constexpr float GRIPPER_OPEN = 0.8F;
constexpr int FITTING_APPROACH = 16;
constexpr float SPARKS_LENGTH = 0.14F;

}  // namespace rtype::client
