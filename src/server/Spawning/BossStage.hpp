#pragma once

#include <cstddef>
#include <optional>
#include "BossStageParts.hpp"
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "EventBus.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief The end of a level: the boss of the difficulty, once its last wave
 * is over.
 *
 * Without a boss, the level is cleared on the tick the last wave is found
 * over. With one, that tick publishes a game::BossArrival on the bus and logs
 * it; BOSS_ARRIVAL_WARNING_TICKS ticks later, the boss appears at
 * BOSS_ENTRY_POSITION (see game::World::spawn) and every connected player is
 * sent its EntitySpawned. The level is cleared on the first tick after that
 * which finds no enemy in the world, the boss and the Bydos of its volleys
 * included: a boss is never removed by leaving, so it is gone only once
 * killed. When every network id was given, no boss appears and a warning is
 * logged. Not thread-safe: use it from the simulation thread only.
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
  void announceArrival();
  void bringTheBossIn();

  game::World* world_;
  PlayerRoster* roster_;
  engine::EventBus* events_;
  const game::EntitySheet* boss_;
  SendDatagram send_;
  logging::Logger logger_;
  std::optional<std::size_t> ticksToArrival_{std::nullopt};
  bool arrived_{false};
  bool cleared_{false};
};

}  // namespace rtype::server
