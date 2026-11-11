#include "BossTally.hpp"
#include <cstdint>
#include <variant>
#include <vector>
#include "BossFight.hpp"
#include "BossStatus.hpp"
#include "BossTallyConstants.hpp"
#include "EntityMessage.hpp"
#include "GameEnded.hpp"
#include "SightFigures.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

namespace {

/** @returns The health @p status says the boss kept, in percent of its full
 * health, rounded up so a boss that is alive never reads 0; 0 when its full
 * health is 0. */
std::uint8_t healthPercentOf(const network::BossStatus& status) {
  if (status.fullHealth == 0) {
    return 0;
  }
  const std::uint32_t scaled =
      static_cast<std::uint32_t>(status.health) * BOSS_FULL_HEALTH_PERCENT;
  return static_cast<std::uint8_t>((scaled + status.fullHealth - 1) /
                                   status.fullHealth);
}

}  // namespace

void BossTally::count(const client::EntityMessage& message) {
  if (const auto* status = std::get_if<network::BossStatus>(&message)) {
    readStatus(*status);
  } else if (std::holds_alternative<network::GameEnded>(message)) {
    endFight();
  }
}

void BossTally::addTime(engine::Duration elapsed, const SightFigures& figures) {
  if (current_.has_value() && bossFights_) {
    if (figures.inPlay) {
      current_->timeAlive += elapsed;
    }
    current_->livesLost += figures.livesLost - previousLivesLost_;
  }
  previousLivesLost_ = figures.livesLost;
}

std::vector<BossFight> BossTally::fights() const { return ended_; }

void BossTally::readStatus(const network::BossStatus& status) {
  if (status.stage == network::BossStage::Arriving) {
    return;
  }
  if (!current_.has_value()) {
    current_ = BossFight{};
  }
  current_->bossHealthLeftPercent = healthPercentOf(status);
  bossFights_ = status.stage != network::BossStage::Defeated;
  current_->defeated = status.stage == network::BossStage::Defeated;
}

void BossTally::endFight() {
  if (current_.has_value()) {
    ended_.push_back(*current_);
  }
  current_.reset();
  bossFights_ = false;
}

}  // namespace rtype::tools
