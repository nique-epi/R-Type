#pragma once

#include <cstdint>
#include <optional>
#include "BossEntranceParts.hpp"
#include "BossHealthScaling.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "EventBus.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/** @returns The points of the sheet of @p scaling scaled for its players
 * (at least one counts) and its health percent: each player past the first
 * adds BOSS_HEALTH_PER_EXTRA_PLAYER percent, then the whole is taken at the
 * health percent, rounded down, never under 1. */
[[nodiscard]] int scaledBossHealth(const BossHealthScaling& scaling);

/**
 * @brief How a boss arrives, at the end of the waves (BossStage) or when a
 * level names it (LevelBosses): the warning the players are sent, then the
 * boss entering.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class BossEntrance {
 public:
  explicit BossEntrance(BossEntranceParts parts);

  /** @brief Publishes the game::BossArrival of @p boss on the bus, which
   * warns the players, and logs it. */
  void announce(const game::EntitySheet& boss) const;

  /**
   * @brief Brings @p boss in at BOSS_ENTRY_POSITION, its health scaled for
   * the players connected and @p healthPercent (see scaledBossHealth()), and
   * sends every connected player its EntitySpawned. Only that boss is
   * scaled: a boss already in the world keeps its health. When every network
   * id was given, no boss appears and a warning is logged.
   */
  void bringIn(const game::EntitySheet& boss, int healthPercent) const;

 private:
  [[nodiscard]] std::optional<engine::Entity> bossWith(
      std::uint32_t networkId) const;
  void strengthen(const game::EntitySheet& boss, engine::Entity entity,
                  int healthPercent) const;
  void tellThePlayers(const game::EntitySheet& boss,
                      std::uint32_t networkId) const;

  game::World* world_;
  PlayerRoster* roster_;
  engine::EventBus* events_;
  SendDatagram send_;
  logging::Logger logger_;
};

}  // namespace rtype::server
