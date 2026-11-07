#include "BossTracker.hpp"
#include <cstdint>
#include <optional>
#include "BossFight.hpp"
#include "BossStatus.hpp"
#include "BossTrackerConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void BossTracker::follow(const network::BossStatus& status,
                         engine::Duration now) {
  if (status.stage == network::BossStage::Arriving) {
    fight_ = BossFight{};
    arrivedAt_ = now;
    return;
  }
  if (!fight_.fightingSince.has_value()) {
    fight_.fightingSince = now;
    fight_.lagHealth = status.health;
  }
  recordLoss(status.health, now);
  fight_.stage = status.stage;
  fight_.health = status.health;
  fight_.fullHealth = status.fullHealth;
  if (!fight_.phaseOneEmptiedAt.has_value() &&
      status.health <= status.fullHealth / BOSS_PHASE_COUNT) {
    fight_.phaseOneEmptiedAt = now;
  }
}

const BossFight& BossTracker::fight() const { return fight_; }

std::optional<engine::Duration> BossTracker::sinceArrival(
    engine::Duration now) const {
  if (!arrivedAt_.has_value()) {
    return std::nullopt;
  }
  return now - *arrivedAt_;
}

void BossTracker::recordLoss(std::uint16_t health, engine::Duration now) {
  if (health >= fight_.health) {
    return;
  }
  const bool lagOpen =
      fight_.lastLossAt.has_value() && now - *fight_.lastLossAt < BOSS_LAG_TIME;
  if (!lagOpen) {
    fight_.lagHealth = fight_.health;
  }
  fight_.lastLossAt = now;
}

}  // namespace rtype::client
