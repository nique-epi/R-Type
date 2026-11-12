#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "PercentConstants.hpp"
#include "WaveCurve.hpp"
#include "WavePlanConstants.hpp"

namespace rtype::server {

/**
 * @brief The waves of one difficulty: how they grow, and the names of the
 * entity sheets their enemies take in turn, the cycle carrying on from one
 * wave to the next.
 */
struct WavePlan {
  game::WaveCurve curve;
  std::vector<std::string> enemies;
  /** @brief The sheet of the boss that comes once the last wave is over;
   * nothing for a level the last wave ends. */
  std::optional<std::string> boss{std::nullopt};
  /** @brief The health of the boss in percent of what its sheet gives,
   * before the players scale it (see BossStage). */
  int bossHealthPercent{game::WHOLE_PERCENT};
  /** @brief The look items that float in at the start of a wave, taken in
   * turn; empty for a level without any. */
  std::vector<std::string> lookItems;
  /** @brief A look item comes with each wave whose number is a multiple of
   * this count, never with the first: 1 for each wave from the second, 3 for
   * the third, the sixth, the ninth. At least 1. */
  std::size_t lookItemEvery{LOOK_ITEM_EVERY_WAVE};

  bool operator==(const WavePlan&) const = default;
};

}  // namespace rtype::server
