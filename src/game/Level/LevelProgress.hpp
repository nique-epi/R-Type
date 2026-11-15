#pragma once

#include <optional>
#include <string>
#include "LevelCheckpoint.hpp"
#include "LevelScroll.hpp"

namespace rtype::game {

/**
 * @brief Where the level of a game stands: its scroll, the biome its players
 * see, empty until the level names one, the last checkpoint it reached,
 * nothing before the first, and whether a stage asked that the enemies the
 * level places come in as the scroll reaches them, which a restart from a
 * checkpoint keeps.
 */
struct LevelProgress {
  LevelScroll scroll;
  std::string biome;
  std::optional<LevelCheckpoint> checkpoint = std::nullopt;
  bool placedEnemiesAsked{false};
};

}  // namespace rtype::game
