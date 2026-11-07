#pragma once

#include <cstddef>
#include "GameFlowFixture.hpp"

namespace rtype::server::testing {

/**
 * @brief A game flow whose level holds one wave, then the warlord of the
 * boss stage tests: the game is won once the warlord is gone with every
 * other enemy.
 */
class GameFlowBossLevelFixture : public GameFlowFixture {
 public:
  GameFlowBossLevelFixture();

 protected:
  /** @returns How many bosses the world of the game holds. */
  [[nodiscard]] std::size_t bossesInPlay();
};

}  // namespace rtype::server::testing
