#pragma once

#include <optional>
#include <string>
#include "LevelCheckpoint.hpp"
#include "LevelScroll.hpp"

namespace rtype::game {

/**
 * @brief Where the level of a game stands: its scroll, the biome its players
 * see, empty until the level names one, and the last checkpoint it reached,
 * nothing before the first.
 */
struct LevelProgress {
  LevelScroll scroll;
  std::string biome;
  std::optional<LevelCheckpoint> checkpoint = std::nullopt;
};

}  // namespace rtype::game
