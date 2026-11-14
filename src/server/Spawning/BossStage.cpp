#include "BossStage.hpp"
#include <string>
#include <utility>
#include "BossEntranceParts.hpp"
#include "BossStageConstants.hpp"
#include "BossStageParts.hpp"
#include "EnemyCount.hpp"
#include "EntitySheet.hpp"
#include "PercentConstants.hpp"
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
      boss_(bossSheetOf(parts)),
      bosses_(BossEntranceParts{.world = parts.world,
                                .roster = parts.roster,
                                .send = std::move(parts.send),
                                .events = parts.events},
              parts.types.get(), parts.healthPercent),
      logger_(std::string{BOSS_STAGE_LOGGER_NAME}) {}

void BossStage::onTick(bool lastWaveFinished) {
  if (!lastWaveFinished || cleared_) {
    return;
  }
  if (boss_ == nullptr) {
    cleared_ = true;
    return;
  }
  if (!announced_) {
    announced_ = true;
    logger_.info("the last wave is over");
    bosses_.announce(boss_->name, game::WHOLE_PERCENT);
    return;
  }
  bosses_.onTick();
  if (bosses_.pending() > 0) {
    return;
  }
  cleared_ = game::enemiesIn(world_->components()) == 0;
}

bool BossStage::levelCleared() const { return cleared_; }

bool BossStage::hasBoss() const { return boss_ != nullptr; }

}  // namespace rtype::server
