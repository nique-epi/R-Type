#include "BossPhaseSystemFixture.hpp"
#include <vector>
#include "Boss.hpp"
#include "BossHealthChanged.hpp"
#include "BossPhaseSystemTestConstants.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStarted.hpp"
#include "Health.hpp"
#include "NetworkIdentity.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

BossPhaseSystemFixture::BossPhaseSystemFixture() {
  events_.subscribe<BossHealthChanged>(
      [this](const BossHealthChanged& health) { healths_.push_back(health); });
  events_.subscribe<EnemyActionStarted>(
      [this](const EnemyActionStarted& started) {
        startedActions_.push_back(started);
      });
}

void BossPhaseSystemFixture::addBoss(Boss boss, int points,
                                     CurrentAction current) {
  boss_ = entities_.create();
  components_.add(boss_, boss);
  components_.add(boss_,
                  Health{.points = points, .full = PHASED_BOSS_FULL_HEALTH});
  components_.add(boss_, current);
  components_.add(boss_, NetworkIdentity{.networkId = PHASED_BOSS_ID});
}

void BossPhaseSystemFixture::run() {
  system_.update(components_, engine::SIMULATION_TICK_DURATION);
  events_.dispatch();
}

Boss BossPhaseSystemFixture::boss() {
  const Boss* const found = components_.get<Boss>(boss_);
  return found == nullptr ? Boss{} : *found;
}

const std::vector<BossHealthChanged>& BossPhaseSystemFixture::healths() const {
  return healths_;
}

const std::vector<EnemyActionStarted>& BossPhaseSystemFixture::startedActions()
    const {
  return startedActions_;
}

}  // namespace rtype::game::testing
