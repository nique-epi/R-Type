#include "BossStage.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include "BossArrival.hpp"
#include "BossStageConstants.hpp"
#include "BossStageParts.hpp"
#include "ConnectedPlayer.hpp"
#include "EnemyCount.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumber.hpp"
#include "EventBus.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @returns The sheet of the boss of @p parts; nullptr for none. */
const game::EntitySheet* bossSheetOf(const BossStageParts& parts) {
  if (!parts.boss.has_value()) {
    return nullptr;
  }
  return &parts.types.get().named(*parts.boss);
}

}  // namespace

BossStage::BossStage(BossStageParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      events_(&parts.events.get()),
      boss_(bossSheetOf(parts)),
      send_(std::move(parts.send)),
      logger_(std::string{BOSS_STAGE_LOGGER_NAME}) {}

void BossStage::onTick(bool lastWaveFinished) {
  if (!lastWaveFinished || cleared_) {
    return;
  }
  if (boss_ == nullptr) {
    cleared_ = true;
    return;
  }
  if (!ticksToArrival_.has_value()) {
    announceArrival();
    return;
  }
  if (*ticksToArrival_ > 0) {
    --*ticksToArrival_;
    return;
  }
  if (!arrived_) {
    bringTheBossIn();
    return;
  }
  cleared_ = game::enemiesIn(world_->components()) == 0;
}

bool BossStage::levelCleared() const { return cleared_; }

bool BossStage::hasBoss() const { return boss_ != nullptr; }

void BossStage::announceArrival() {
  ticksToArrival_ = BOSS_ARRIVAL_WARNING_TICKS;
  events_->publish(game::BossArrival{
      .typeNumber = boss_->number,
      .warningMilliseconds = BOSS_ARRIVAL_WARNING_MILLISECONDS});
  logger_.info("the last wave is over: ", boss_->name, " arrives in ",
               BOSS_ARRIVAL_WARNING_MILLISECONDS, " ms");
}

void BossStage::bringTheBossIn() {
  arrived_ = true;
  std::uint32_t networkId = 0;
  try {
    networkId = world_->spawn(*boss_, BOSS_ENTRY_POSITION);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no boss appears: every network id was given");
    return;
  }
  const network::EntitySpawned spawned = entitySpawnedOf(
      networkId, game::EntityTypeNumber{.number = boss_->number},
      BOSS_ENTRY_POSITION);
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(send_, *player, spawned);
  }
}

}  // namespace rtype::server
