#pragma once

#include <string>
#include "BossStageParts.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief The end of a game that plays the waves of its difficulty: the boss
 * of the difficulty, once the last wave is over.
 *
 * Without a boss, the level is cleared on the tick the last wave is found
 * over. With one, that tick orders it at BOSS_ENTRY_POSITION, as a script
 * orders a body: it appears at the end of the next tick, and game::BossSystem
 * warns the players of it then, as of any boss. The level is cleared on the
 * first tick after the order which finds no enemy in the world, the boss and
 * the Bydos of its volleys included: a boss is never removed by leaving, so
 * it is gone only once killed. When the boss cannot be ordered, the journal
 * says why, and the level is cleared at the next tick without an enemy.
 * Not thread-safe: use it from the simulation thread only.
 */
class BossStage {
 public:
  explicit BossStage(BossStageParts parts);

  /** @brief Lets one tick pass, the last wave being over or not as
   * @p lastWaveFinished says. */
  void onTick(bool lastWaveFinished);

  /** @returns Whether the level is cleared: its last wave over, and its boss,
   * if it has one, gone with every other enemy. It stays true. */
  [[nodiscard]] bool levelCleared() const;

  /** @returns Whether the level ends with a boss. */
  [[nodiscard]] bool hasBoss() const;

 private:
  /** @brief Orders @p boss, the boss of the level, and says so in the
   * journal. */
  void orderTheBoss(const std::string& boss);

  BossStageParts parts_;
  logging::Logger logger_;
  bool ordered_{false};
  bool cleared_{false};
};

}  // namespace rtype::server
