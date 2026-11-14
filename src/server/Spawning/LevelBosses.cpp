#include "LevelBosses.hpp"
#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>
#include "BossEntranceParts.hpp"
#include "BossStageConstants.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "PendingBoss.hpp"
#include "PercentConstants.hpp"

namespace rtype::server {

LevelBosses::LevelBosses(BossEntranceParts entrance,
                         const game::EntitySheets& types, int difficultyPercent)
    : entrance_(std::move(entrance)),
      types_(&types),
      difficultyPercent_(difficultyPercent) {}

bool LevelBosses::isBoss(std::string_view name) const {
  for (const game::EntitySheet& sheet : types_->all()) {
    if (sheet.name == name) {
      return sheet.boss.has_value();
    }
  }
  return false;
}

void LevelBosses::announce(std::string_view name, int healthPercent) {
  const game::EntitySheet& sheet = types_->named(name);
  entrance_.announce(sheet);
  pending_.push_back(PendingBoss{
      .sheet = &sheet,
      .healthPercent =
          std::max(1, healthPercent * difficultyPercent_ / game::WHOLE_PERCENT),
      .ticksLeft = BOSS_ARRIVAL_WARNING_TICKS});
}

std::size_t LevelBosses::pending() const { return pending_.size(); }

void LevelBosses::onTick() {
  std::vector<PendingBoss> due;
  std::erase_if(pending_, [&due](PendingBoss& boss) {
    if (boss.ticksLeft == 0) {
      due.push_back(boss);
      return true;
    }
    --boss.ticksLeft;
    return false;
  });
  for (const PendingBoss& boss : due) {
    entrance_.bringIn(*boss.sheet, boss.healthPercent);
  }
}

void LevelBosses::forget() { pending_.clear(); }

}  // namespace rtype::server
