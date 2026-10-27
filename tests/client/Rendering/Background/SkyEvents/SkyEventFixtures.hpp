#pragma once

#include <cstdint>
#include <random>
#include "ISkyEvent.hpp"
#include "RecordingPixelSurface.hpp"
#include "SkyLight.hpp"

/** @returns A generator of random numbers seeded with @p seed, for the draws of
 * an event. */
[[nodiscard]] std::mt19937 generatorFrom(std::uint32_t seed);

/** @brief Advances @p event by @p eventSeconds of the sky event clock, in
 * frames of FRAME, the last one whole. */
void advanceFor(rtype::client::ISkyEvent& event, float eventSeconds);

/** @returns The pixels @p event paints, in order, in a sky of that light. */
[[nodiscard]] RecordingPixelSurface paintOf(
    const rtype::client::ISkyEvent& event, rtype::client::SkyLight light);
