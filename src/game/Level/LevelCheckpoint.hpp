#pragma once

#include <cstddef>
#include <string>
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief Where a team that lost every life starts again: the stage of the
 * level that begins there, counted from 0, the time the level had scrolled,
 * and the biome shown when that stage began with the position of the scroll
 * when that biome began, in logical units.
 */
struct LevelCheckpoint {
  std::size_t stage{};
  engine::Duration travelled{};
  std::string biome;
  float biomeSince{};

  bool operator==(const LevelCheckpoint&) const = default;
};

}  // namespace rtype::game
