#pragma once

#include "BossStageParts.hpp"
#include "EntitySheet.hpp"
#include "LevelBosses.hpp"
#include "Logger.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief The end of a level: the boss of the difficulty, once its last wave
 * is over.
 *
 * Without a boss, the level is cleared on the tick the last wave is found
 * over. With one, that tick announces it, and it enters
 * BOSS_ARRIVAL_WARNING_TICKS ticks later, on the next tick, as the boss of a
 * level does (see LevelBosses). The level is cleared on the first tick after
 * that which finds no enemy in the world, the boss and the Bydos of its volleys
 * included: a boss is never removed by leaving, so it is gone only once
 * killed. Not thread-safe: use it from the simulation thread only.
 */
class BossStage {
 public:
  /** @throws game::UnknownEntityTypeException when the boss of @p parts
   * names no sheet; WaveCatalog already refused a waves file that does. */
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
  game::World* world_;
  const game::EntitySheet* boss_;
  LevelBosses bosses_;
  logging::Logger logger_;
  bool announced_{false};
  bool cleared_{false};
};

}  // namespace rtype::server
