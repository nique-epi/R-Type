#pragma once

#include <cstdint>
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief What one game is set up with: the seed its random draws start from,
 * and the waves of its difficulty.
 */
struct GameSettings {
  std::uint32_t seed{};
  WavePlan waves;
};

}  // namespace rtype::server
