#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::client {

/**
 * @brief Where the server said an entity was at one of its ticks.
 */
struct PositionSample {
  std::uint32_t tick{};
  game::Position position{};
};

}  // namespace rtype::client
