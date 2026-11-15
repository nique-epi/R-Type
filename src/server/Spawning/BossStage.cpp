#include "BossStage.hpp"
#include <string>
#include <utility>
#include "BossStageConstants.hpp"
#include "BossStageParts.hpp"
#include "EnemyCount.hpp"
#include "EngineException.hpp"
#include "SpawnOptions.hpp"

namespace rtype::server {

BossStage::BossStage(BossStageParts parts)
    : parts_(std::move(parts)), logger_(std::string{BOSS_STAGE_LOGGER_NAME}) {}

void BossStage::onTick(bool lastWaveFinished) {
  if (!lastWaveFinished || cleared_) {
    return;
  }
  if (!parts_.boss.has_value()) {
    cleared_ = true;
    return;
  }
  if (!ordered_) {
    ordered_ = true;
    orderTheBoss(*parts_.boss);
    return;
  }
  cleared_ = game::enemiesIn(parts_.world.get().components()) == 0;
}

bool BossStage::levelCleared() const { return cleared_; }

bool BossStage::hasBoss() const { return parts_.boss.has_value(); }

void BossStage::orderTheBoss(const std::string& boss) {
  logger_.info("the last wave is over: ", boss, " comes");
  try {
    static_cast<void>(parts_.orders.get().spawn(boss, BOSS_ENTRY_POSITION,
                                                game::SpawnOptions{}));
  } catch (const engine::EngineException& error) {
    logger_.warn("no boss appears: ", error.what());
  }
}

}  // namespace rtype::server
