#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "ServerConstants.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief What one game is set up with: the seed its random draws start from,
 * the waves of its difficulty, the world states it sends per second, the ticks
 * it holds each input and the name of its difficulty.
 */
struct GameSettings {
  std::uint32_t seed{};
  WavePlan waves;
  std::size_t statesPerSecond{DEFAULT_WORLD_STATES_PER_SECOND};
  std::size_t inputDelayTicks{DEFAULT_INPUT_DELAY_TICKS};
  /** @brief The name the results of the game are filed under; waves holds the
   * waves of that difficulty, not its name. */
  std::string difficulty{DEFAULT_DIFFICULTY};
};

}  // namespace rtype::server
