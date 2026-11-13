#pragma once

#include <optional>
#include <string>
#include "LevelCheckpoint.hpp"
#include "LevelScroll.hpp"

namespace rtype::game {

/**
 * @brief Where the level of a game stands: its scroll, the biome its players
 * see, empty until the level names one, the position of the scroll when that
 * biome began, in logical units, and the last checkpoint it reached, nothing
 * before the first.
 */
struct LevelProgress {
  LevelScroll scroll;
  std::string biome;
  float biomeSince{};
  std::optional<LevelCheckpoint> checkpoint = std::nullopt;
};

}  // namespace rtype::game
