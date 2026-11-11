#pragma once

#include <cstdint>
#include "BossTallyConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief One fight of a bot with the boss of a game, from the first BossStatus
 * that says the boss fights to the end of that game.
 */
struct BossFight {
  /** @brief Whether the boss was destroyed. */
  bool defeated{false};
  /** @brief The health the boss kept, in percent of its full health, as its
   * newest BossStatus said it: never 0 for a boss still alive, rounded up, and
   * 0 once it is destroyed. */
  std::uint8_t bossHealthLeftPercent{BOSS_FULL_HEALTH_PERCENT};
  /** @brief How long the bot had a life left while the boss fought. */
  engine::Duration timeAlive{};
  /** @brief The lives the bot lost while the boss fought, not those it lost
   * before: a bot that arrived with fewer lives is out after fewer. */
  std::uint32_t livesLost{0};
};

}  // namespace rtype::tools
