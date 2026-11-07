#pragma once

#include <chrono>
#include <cstdint>
#include <numbers>
#include "TimeConstants.hpp"
#include "WarningBannerConstants.hpp"

namespace rtype::client {

/** @brief The edges of the playfield tinted HAZARD_RED during the WARNING
 * screen, from clear to EDGE_PULSE_PEAK_ALPHA (70 %) and back once every
 * EDGE_PULSE_PERIOD along a cosine; strongest on the edge, fading to nothing
 * EDGE_PULSE_DEPTH pixels in, by strips EDGE_PULSE_STEP wide. */
constexpr engine::Duration EDGE_PULSE_PERIOD = std::chrono::milliseconds(800);
constexpr std::uint8_t EDGE_PULSE_PEAK_ALPHA = 178;
constexpr int EDGE_PULSE_DEPTH = 56;
constexpr int EDGE_PULSE_STEP = 2;
constexpr double EDGE_PULSE_FULL_TURN = 2.0 * std::numbers::pi;
constexpr double EDGE_PULSE_HALF = 0.5;

/** @brief The roar of the boss tints the edges ROAR_EDGE_FLASH_ALPHA (40 %),
 * fading to clear over ROAR_EDGE_FLASH_TIME. */
constexpr std::uint8_t ROAR_EDGE_FLASH_ALPHA = 102;
constexpr engine::Duration ROAR_EDGE_FLASH_TIME =
    std::chrono::milliseconds(400);

static_assert(EDGE_PULSE_DEPTH % EDGE_PULSE_STEP == 0,
              "the strips cover the depth exactly");
static_assert(EDGE_PULSE_PERIOD >= std::chrono::milliseconds(500),
              "the edges pulse at 2 Hz at most");

}  // namespace rtype::client
