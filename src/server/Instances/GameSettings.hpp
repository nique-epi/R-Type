#pragma once

#include <cstddef>
#include <cstdint>
#include "ServerConstants.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief What one game is set up with: the seed its random draws start from,
 * the waves of its difficulty, the world states it sends per second and the
 * ticks it holds each input.
 */
struct GameSettings {
  std::uint32_t seed{};
  WavePlan waves;
  std::size_t statesPerSecond{DEFAULT_WORLD_STATES_PER_SECOND};
  std::size_t inputDelayTicks{DEFAULT_INPUT_DELAY_TICKS};
};

}  // namespace rtype::server
