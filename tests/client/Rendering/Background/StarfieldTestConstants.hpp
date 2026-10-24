#pragma once

#include <chrono>
#include "BackgroundConstants.hpp"
#include "StarTwinkleStep.hpp"
#include "TimeConstants.hpp"

/** @brief A frame of ten milliseconds, and frames half as long as the
 * longest step the background takes at once. */
constexpr rtype::engine::Duration SHORT_STEP = std::chrono::milliseconds(10);
constexpr rtype::engine::Duration HALF_MAXIMUM_STEP =
    rtype::client::MAXIMUM_BACKGROUND_STEP / 2;

/** @brief A minute, passed frame by frame. */
constexpr rtype::engine::Duration ONE_MINUTE = std::chrono::minutes(1);

/** @brief A frame of negative time. */
constexpr rtype::engine::Duration NEGATIVE_STEP =
    -std::chrono::milliseconds(10);

/** @brief One twinkle step of the stars, and half of it: the middle of a
 * step. */
constexpr rtype::engine::Duration ONE_TWINKLE_STEP =
    std::chrono::duration_cast<rtype::engine::Duration>(
        rtype::client::StarTwinkleStep(1));
constexpr rtype::engine::Duration HALF_TWINKLE_STEP = ONE_TWINKLE_STEP / 2;

/** @brief More time than the planet takes to drift past the left edge. */
constexpr rtype::engine::Duration LONGER_THAN_A_PLANET_CROSSING =
    std::chrono::seconds(260);

/** @brief How far a star may be from where it must be: positions are
 * computed in float. */
constexpr float POSITION_TOLERANCE = 1e-3F;

/** @brief The fastest speed factor that paints no streak, and the slowest
 * that paints them. */
constexpr float LAST_FACTOR_WITHOUT_STREAK = 1.5F;
constexpr float FIRST_FACTOR_WITH_STREAKS = 1.6F;

/** @brief Pixels of streak at the right of a near star at the top of the
 * warp: the distance it covers in a sixtieth of a second. */
constexpr int NEAR_STREAK_AT_THE_TOP_WARP = 6;

/** @brief A column far enough from the left edge that a near star there, at
 * the top of the warp, does not wrap during the longest step: it moves 18
 * pixels. */
constexpr float FAR_FROM_THE_LEFT_EDGE = 40.0F;
