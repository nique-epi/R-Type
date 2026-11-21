#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "GameRuleConstants.hpp"
#include "LevelCheckpoint.hpp"
#include "LevelScroll.hpp"
#include "LevelTransition.hpp"

namespace rtype::game {

/**
 * @brief Where the level of a game stands: its scroll, the biome its players
 * see, empty until the level names one, the position of the scroll when that
 * biome began, in logical units, the last checkpoint it reached, nothing
 * before the first, and whether a stage asked that the enemies the level
 * places come in as the scroll reaches them, which a restart from a
 * checkpoint keeps, the transition it plays, nothing while it plays none,
 * and the times its team may still go back to its checkpoint after losing
 * every life.
 */
struct LevelProgress {
  LevelScroll scroll;
  std::string biome;
  float biomeSince{};
  std::optional<LevelCheckpoint> checkpoint = std::nullopt;
  bool placedEnemiesAsked{false};
  std::optional<LevelTransition> transition = std::nullopt;
  std::uint8_t continuesLeft{identifiers::TEAM_CONTINUES};
};

}  // namespace rtype::game
