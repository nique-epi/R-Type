#pragma once

#include <cstdint>
#include <optional>
#include "Position.hpp"

namespace rtype::client {

/** @brief The figure of a gain. */
struct GainFigure {
  game::Position place{};
  std::uint32_t points{};
  /** @brief 0 to PLAYER_COLOR_COUNT - 1; nothing when several pilots
   * gained, or one gained for several kills. */
  std::optional<int> pilotColor;
  bool big = false;
};

}  // namespace rtype::client
