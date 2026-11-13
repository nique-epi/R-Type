#pragma once

#include <cstddef>
#include <string_view>
#include <vector>
#include "BossEntrance.hpp"
#include "BossEntranceParts.hpp"
#include "EntitySheets.hpp"
#include "ILevelBosses.hpp"
#include "PendingBoss.hpp"

namespace rtype::server {

/**
 * @brief The bosses the level of a game brings in (see `level.boss`): each
 * one is announced at once, then enters BOSS_ARRIVAL_WARNING_TICKS ticks
 * later, as the boss of the waves does (see BossEntrance).
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class LevelBosses : public game::ILevelBosses {
 public:
  /**
   * @param types The sheets the bosses are named among; they must outlive the
   * object.
   * @param difficultyPercent The health of every boss in percent of its sheet
   * for the difficulty (WavePlan::bossHealthPercent), which the percent each
   * announce asks for multiplies.
   */
  LevelBosses(BossEntranceParts entrance, const game::EntitySheets& types,
              int difficultyPercent);

  [[nodiscard]] bool isBoss(std::string_view name) const override;
  void announce(std::string_view name, int healthPercent) override;
  [[nodiscard]] std::size_t pending() const override;

  /** @brief Lets one tick pass: each boss whose warning is over enters. */
  void onTick();

  /** @brief Forgets the bosses announced that have not entered, as a restart
   * from a checkpoint removes the bodies of the level. */
  void forget();

 private:
  BossEntrance entrance_;
  const game::EntitySheets* types_;
  int difficultyPercent_;
  std::vector<PendingBoss> pending_;
};

}  // namespace rtype::server
