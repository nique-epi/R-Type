#pragma once

#include <cstddef>
#include <cstdint>
#include "ServerConstants.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief What one game is set up with: the seed its random draws start from,
 * the waves of its difficulty and the world states it sends per second.
 */
struct GameSettings {
  std::uint32_t seed{};
  WavePlan waves;
  std::size_t statesPerSecond{DEFAULT_WORLD_STATES_PER_SECOND};
};

}  // namespace rtype::server
