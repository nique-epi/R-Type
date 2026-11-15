#pragma once

#include <cstddef>
#include <cstdint>

/** @brief Seed the tests of the background give the draws of a sky, so that
 * two skies of the same seed hold the same stars. */
constexpr std::uint32_t SEED = 2026;

/** @brief The ranks of the planes among the layers of the Pit, from the
 * farthest to the nearest. */
constexpr std::size_t FAR_PLANE = 0;
constexpr std::size_t MIDDLE_PLANE = 1;
constexpr std::size_t NEAR_PLANE = 2;
