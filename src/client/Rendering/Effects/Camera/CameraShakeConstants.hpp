#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief The shake, after Eiserloh (GDC 2016): SHAKE_MAXIMUM_OFFSET pixels of
 * the canvas times the trauma to the power SHAKE_EXPONENT, so a small trauma
 * barely shows, times a smoothed noise of SHAKE_NOISE_STEPS_PER_SECOND steps
 * a second. The trauma loses TRAUMA_DECAY_PER_SECOND a second.
 */
constexpr float SHAKE_MAXIMUM_OFFSET = 6.0F;
constexpr float SHAKE_EXPONENT = 2.0F;
constexpr double SHAKE_NOISE_STEPS_PER_SECOND = 20.0;
constexpr float TRAUMA_DECAY_PER_SECOND = 1.5F;

/**
 * @brief Trauma of every fifth kill of a chain, at most 1.2 pixels, and of
 * any ship destroyed, the player's own included, at most 0.54 pixel: both
 * round to 0 or 1 pixel.
 */
constexpr float BIG_CATCH_TRAUMA = 0.45F;
constexpr float SHIP_DOWN_TRAUMA = 0.3F;

/** @brief Seeds of the noise of each axis, so the two axes do not move
 * together. */
constexpr std::uint32_t SHAKE_SEED_X = 1;
constexpr std::uint32_t SHAKE_SEED_Y = 2;

/** @brief The integer hash that gives a value to each step of the noise: the
 * golden ratio, then the finalizer of MurmurHash3. */
constexpr std::uint32_t NOISE_STEP_MULTIPLIER = 0x9E3779B1U;
constexpr std::uint32_t NOISE_SEED_MULTIPLIER = 0x85EBCA6BU;
constexpr std::uint32_t NOISE_FINAL_MULTIPLIER = 0xC2B2AE35U;
constexpr std::uint32_t NOISE_SHIFT = 16;

}  // namespace rtype::client
