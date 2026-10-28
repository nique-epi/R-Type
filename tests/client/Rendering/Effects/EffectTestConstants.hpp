#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

/** @brief Lengths of the effects, as the animation page shows them. */
constexpr rtype::engine::Duration BYDO_EXPLOSION_LENGTH =
    std::chrono::milliseconds(420);
constexpr rtype::engine::Duration SHIP_EXPLOSION_LENGTH =
    std::chrono::milliseconds(600);
constexpr rtype::engine::Duration IMPACT_LENGTH =
    std::chrono::milliseconds(150);
constexpr rtype::engine::Duration MUZZLE_FLASH_LENGTH =
    std::chrono::milliseconds(100);
constexpr rtype::engine::Duration HIT_FLASH_LENGTH =
    std::chrono::milliseconds(60);

/** @brief The freezes, moves and waits of the effects, as the cards of the
 * effects give them. */
constexpr rtype::engine::Duration KILL_HOLD_LENGTH =
    std::chrono::milliseconds(50);
constexpr rtype::engine::Duration BIG_CATCH_HOLD_LENGTH =
    std::chrono::milliseconds(83);
constexpr rtype::engine::Duration SHIP_DOWN_HOLD_LENGTH =
    std::chrono::milliseconds(100);
constexpr rtype::engine::Duration MATERIALIZE_LENGTH =
    std::chrono::milliseconds(267);
constexpr rtype::engine::Duration PILOT_ARRIVAL_WAIT =
    std::chrono::milliseconds(500);
constexpr rtype::engine::Duration CHAIN_WINDOW_LENGTH = std::chrono::seconds(1);
constexpr rtype::engine::Duration GAIN_WINDOW_LENGTH =
    std::chrono::milliseconds(150);
constexpr rtype::engine::Duration POPUP_RISE_LENGTH =
    std::chrono::milliseconds(400);
constexpr rtype::engine::Duration POPUP_STILL_LENGTH =
    std::chrono::milliseconds(700);
constexpr rtype::engine::Duration POPUP_BLINK_LENGTH =
    std::chrono::milliseconds(67);
constexpr rtype::engine::Duration POPUP_LIFE_LENGTH =
    std::chrono::milliseconds(968);
constexpr rtype::engine::Duration SHOT_CUE_LIFE_LENGTH =
    std::chrono::milliseconds(500);

/** @brief The life of a particle the tests make: a drop, as the card of
 * the debris gives it. */
constexpr rtype::engine::Duration A_PARTICLE_LIFE =
    std::chrono::milliseconds(400);

/** @brief MISSION START, as its card gives it: the stars reach 12 times
 * their speed at 117 ms, are back to their own speed at 600 ms, and the
 * banner writes its first letter at 1 s. */
constexpr rtype::engine::Duration WARP_TOP_TIME =
    std::chrono::milliseconds(117);
constexpr rtype::engine::Duration WARP_END_TIME =
    std::chrono::milliseconds(600);
constexpr float OWN_SPEED = 1.0F;
constexpr float TOP_WARP_SPEED = 12.0F;

/** @brief How far a speed factor of the warp may be from what it must be:
 * the warp is computed in float. */
constexpr float WARP_FACTOR_TOLERANCE = 1e-4F;
constexpr rtype::engine::Duration BANNER_START_TIME = std::chrono::seconds(1);

/** @brief Seed the tests give the draws of the particles. */
constexpr std::uint32_t EFFECT_TEST_SEED = 2026;

/** @brief Counts of the effects, as the cards give them. */
constexpr std::size_t KILL_DROP_COUNT = 8;
constexpr std::size_t BIG_CATCH_DROP_COUNT = 16;
constexpr std::size_t SHIP_DEBRIS_COUNT = 24;
constexpr std::size_t SPARK_COUNT = 3;
constexpr std::size_t MOST_PARTICLES = 192;
constexpr int FLASH_PIXELS_ALLOWED = 3600;
constexpr int KILLS_PER_BIG_CATCH = 5;

/** @brief Two kills 100 ms apart: well inside a chain. */
constexpr rtype::engine::Duration CLOSE_KILLS = std::chrono::milliseconds(100);

/** @brief A score a pilot had, and the points a drifter gives when it dies. */
constexpr std::uint32_t SCORE_BEFORE = 1000;
constexpr std::uint32_t DRIFTER_POINTS = 100;

/** @brief Logical units of the playfield in one pixel of the canvas. */
constexpr float UNITS_PER_PIXEL = 4.0F;

/** @brief How high a figure rises, 8 pixels, in logical units. */
constexpr float POPUP_RISE_UNITS = 32.0F;
