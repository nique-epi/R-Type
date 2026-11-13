#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include "ShopkeeperEye.hpp"
#include "ShopkeeperEyeCue.hpp"
#include "ShopkeeperPose.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The shopkeeper moves by steps of SHOPKEEPER_STEP, a whole number
 * of the ticks of the clock, so his cycles are counted in whole steps. */
constexpr engine::Duration SHOPKEEPER_STEP = std::chrono::milliseconds(125);

/** @brief His breath: how far under his place of rest he is drawn at each
 * step of a cycle of 24 steps (3 s), a sine of 2 pixels rounded to whole
 * pixels, so he sinks, comes back and rises one pixel at a time. */
constexpr std::array<int, 24> SHOPKEEPER_DROPS{0,  1,  1,  1,  2,  2,  2,  2,
                                               2,  1,  1,  1,  0,  -1, -1, -1,
                                               -2, -2, -2, -2, -2, -1, -1, -1};

/** @brief His eye, over a cycle of SHOPKEEPER_EYE_CYCLE_STEPS steps (8 s):
 * it blinks (half closed, closed, half closed, a step each), glances to his
 * left for a second, then to his right, and blinks again before the cycle
 * starts over. The cues are in the order of their steps. */
constexpr std::size_t SHOPKEEPER_EYE_CYCLE_STEPS = 64;
constexpr std::array SHOPKEEPER_EYE_CUES{
    ShopkeeperEyeCue{.step = 0, .eye = ShopkeeperEye::Ahead},
    ShopkeeperEyeCue{.step = 14, .eye = ShopkeeperEye::HalfClosed},
    ShopkeeperEyeCue{.step = 15, .eye = ShopkeeperEye::Closed},
    ShopkeeperEyeCue{.step = 16, .eye = ShopkeeperEye::HalfClosed},
    ShopkeeperEyeCue{.step = 17, .eye = ShopkeeperEye::Ahead},
    ShopkeeperEyeCue{.step = 26, .eye = ShopkeeperEye::Left},
    ShopkeeperEyeCue{.step = 34, .eye = ShopkeeperEye::Ahead},
    ShopkeeperEyeCue{.step = 44, .eye = ShopkeeperEye::Right},
    ShopkeeperEyeCue{.step = 50, .eye = ShopkeeperEye::Ahead},
    ShopkeeperEyeCue{.step = 57, .eye = ShopkeeperEye::HalfClosed},
    ShopkeeperEyeCue{.step = 58, .eye = ShopkeeperEye::Closed},
    ShopkeeperEyeCue{.step = 59, .eye = ShopkeeperEye::HalfClosed},
    ShopkeeperEyeCue{.step = 60, .eye = ShopkeeperEye::Ahead}};

/** @brief The shopkeeper of a player who asked for less motion: at his place
 * of rest, looking ahead. */
constexpr ShopkeeperPose SHOPKEEPER_AT_REST{.drop = 0,
                                            .eye = ShopkeeperEye::Ahead};

}  // namespace rtype::client
