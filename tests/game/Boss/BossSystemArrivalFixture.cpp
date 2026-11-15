#include "BossSystemArrivalFixture.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include "Boss.hpp"
#include "BossArrival.hpp"
#include "BossProgress.hpp"
#include "BossSystem.hpp"
#include "BossSystemArrivalTestConstants.hpp"
#include "BossSystemParts.hpp"
#include "CurrentAction.hpp"
#include "EntityTypeNumber.hpp"
#include "Health.hpp"
#include "NetworkIdentity.hpp"
#include "PercentConstants.hpp"
#include "Summoning.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

BossSystemArrivalFixture::BossSystemArrivalFixture() {
  events_.subscribe<BossArrival>(
      [this](const BossArrival& arrival) { arrivals_.push_back(arrival); });
  useHealthPercent(WHOLE_PERCENT);
}

void BossSystemArrivalFixture::useHealthPercent(int healthPercent) {
  system_.reset();
  system_ = std::make_unique<BossSystem>(
      BossSystemParts{.events = events_,
                      .players = [this] { return players_; },
                      .healthPercent = healthPercent});
}

void BossSystemArrivalFixture::connect(std::size_t players) {
  players_ = players;
}

void BossSystemArrivalFixture::addNewBoss() {
  boss_ = entities_.create();
  components_.add(boss_, Boss{});
  components_.add(boss_, Health{.points = ARRIVING_BOSS_HEALTH,
                                .full = ARRIVING_BOSS_HEALTH});
  components_.add(boss_, CurrentAction{});
  components_.add(boss_, NetworkIdentity{.networkId = ARRIVING_BOSS_ID});
  components_.add(boss_,
                  EntityTypeNumber{.number = ARRIVING_BOSS_SHEET_NUMBER});
}

void BossSystemArrivalFixture::run() {
  system_->update(components_, engine::SIMULATION_TICK_DURATION);
  events_.dispatch();
}

void BossSystemArrivalFixture::endTheWarning() {
  components_.remove<Summoning>(boss_);
}

const std::vector<BossArrival>& BossSystemArrivalFixture::arrivals() const {
  return arrivals_;
}

std::optional<std::size_t> BossSystemArrivalFixture::summoningTicksLeft() {
  const Summoning* const summoning = components_.get<Summoning>(boss_);
  if (summoning == nullptr) {
    return std::nullopt;
  }
  return summoning->ticksLeft;
}

Health BossSystemArrivalFixture::health() {
  const Health* const found = components_.get<Health>(boss_);
  return found == nullptr ? Health{} : *found;
}

BossProgress BossSystemArrivalFixture::progress() {
  const Boss* const found = components_.get<Boss>(boss_);
  return found == nullptr ? BossProgress::Created : found->progress;
}

}  // namespace rtype::game::testing
