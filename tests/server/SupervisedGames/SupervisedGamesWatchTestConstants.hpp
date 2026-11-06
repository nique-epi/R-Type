#pragma once

#include <cstdint>

/** @brief How many lists or measures were published: once, then once more. */
constexpr std::uint64_t PUBLISHED_ONCE = 1;
constexpr std::uint64_t PUBLISHED_TWICE = 2;

/** @brief A median tick a game reports, in microseconds. */
constexpr std::uint32_t REPORTED_MEDIAN_MICROSECONDS = 2500;

/** @brief The seats free beside one game that starts with nobody: all 32. */
constexpr std::uint16_t SEATS_BESIDE_A_STARTING_GAME = 32;
