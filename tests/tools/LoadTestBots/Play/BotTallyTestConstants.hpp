#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

/** @brief Three ticks in a row, as a game sends its world states. */
constexpr std::uint32_t FIRST_TICK = 5;
constexpr std::uint32_t SECOND_TICK = 6;
constexpr std::uint32_t THIRD_TICK = 7;

constexpr std::size_t TWO_WORLD_STATES = 2;
constexpr std::size_t THREE_WORLD_STATES = 3;

/** @brief A frame of the bots, and two of them in a row. */
constexpr engine::Duration A_FRAME = std::chrono::milliseconds{16};
constexpr engine::Duration TWO_FRAMES = std::chrono::milliseconds{32};

/** @brief What the session says of the game when the tally adds a frame. */
constexpr bool THE_GAME_RUNS = true;
constexpr bool THE_SESSION_ENDED_IT = false;

}  // namespace rtype::tools::testing
