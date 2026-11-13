#include "BossEntrance.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include "Boss.hpp"
#include "BossArrival.hpp"
#include "BossEntranceParts.hpp"
#include "BossHealthScaling.hpp"
#include "BossStageConstants.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumber.hpp"
#include "Health.hpp"
#include "NetworkIdentity.hpp"
#include "PercentConstants.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

int scaledBossHealth(const BossHealthScaling& scaling) {
  const auto extraPlayers =
      static_cast<int>(std::max<std::size_t>(scaling.players, 1) - 1);
  const int forPlayers =
      scaling.sheetPoints *
      (game::WHOLE_PERCENT + (BOSS_HEALTH_PER_EXTRA_PLAYER * extraPlayers)) /
      game::WHOLE_PERCENT;
  return std::max(1, forPlayers * scaling.healthPercent / game::WHOLE_PERCENT);
}

BossEntrance::BossEntrance(BossEntranceParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      events_(&parts.events.get()),
      send_(std::move(parts.send)),
      logger_(std::string{BOSS_ENTRANCE_LOGGER_NAME}) {}

void BossEntrance::announce(const game::EntitySheet& boss) const {
  events_->publish(game::BossArrival{
      .typeNumber = boss.number,
      .warningMilliseconds = BOSS_ARRIVAL_WARNING_MILLISECONDS});
  logger_.info(boss.name, " arrives in ", BOSS_ARRIVAL_WARNING_MILLISECONDS,
               " ms");
}

void BossEntrance::bringIn(const game::EntitySheet& boss,
                           int healthPercent) const {
  std::uint32_t networkId = 0;
  try {
    networkId = world_->spawn(boss, BOSS_ENTRY_POSITION);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no boss appears: every network id was given");
    return;
  }
  const std::optional<engine::Entity> entity = bossWith(networkId);
  if (entity.has_value()) {
    strengthen(boss, *entity, healthPercent);
  }
  tellThePlayers(boss, networkId);
}

std::optional<engine::Entity> BossEntrance::bossWith(
    std::uint32_t networkId) const {
  std::optional<engine::Entity> found;
  world_->components().forEach<game::NetworkIdentity, game::Boss>(
      [&found, networkId](engine::Entity entity,
                          const game::NetworkIdentity& identity,
                          const game::Boss&) {
        if (identity.networkId == networkId) {
          found = entity;
        }
      });
  return found;
}

void BossEntrance::strengthen(const game::EntitySheet& boss,
                              engine::Entity entity, int healthPercent) const {
  if (!boss.boss.has_value()) {
    return;
  }
  engine::ComponentRegistry& components = world_->components();
  auto* const health = components.get<game::Health>(entity);
  auto* const phase = components.get<game::Boss>(entity);
  if (health == nullptr || phase == nullptr) {
    return;
  }
  const auto players =
      static_cast<std::size_t>(std::ranges::distance(roster_->connected()));
  health->full =
      scaledBossHealth(BossHealthScaling{.sheetPoints = health->full,
                                         .players = players,
                                         .healthPercent = healthPercent});
  health->points = health->full;
  phase->rageBelowPoints =
      game::rageBelowPointsOf(health->full, boss.boss->rageBelowPercent);
  logger_.info(boss.name, " has ", health->full, " health points for ", players,
               " players");
}

void BossEntrance::tellThePlayers(const game::EntitySheet& boss,
                                  std::uint32_t networkId) const {
  const network::EntitySpawned spawned =
      entitySpawnedOf(networkId, game::EntityTypeNumber{.number = boss.number},
                      BOSS_ENTRY_POSITION);
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(send_, *player, spawned);
  }
}

}  // namespace rtype::server
