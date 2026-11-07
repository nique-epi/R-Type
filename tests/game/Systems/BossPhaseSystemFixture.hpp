#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "Boss.hpp"
#include "BossHealthChanged.hpp"
#include "BossPhaseSystem.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStarted.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EventBus.hpp"
#include "Health.hpp"

namespace rtype::game::testing {

/**
 * @brief A boss moved through its phases by a BossPhaseSystem, recording the
 * health and the actions it announces, once the bus delivered them.
 */
class BossPhaseSystemFixture : public ::testing::Test {
 protected:
  BossPhaseSystemFixture();

  /** @brief Adds the boss PHASED_BOSS_ID: @p boss, with @p points of its
   * full health, in the action @p current. */
  void addBoss(Boss boss, int points, CurrentAction current);

  /** @brief Runs the system once, then delivers what it published. */
  void run();

  /** @returns The Boss of the boss; a calm one when it has none. */
  [[nodiscard]] Boss boss();

  [[nodiscard]] const std::vector<BossHealthChanged>& healths() const;
  [[nodiscard]] const std::vector<EnemyActionStarted>& startedActions() const;

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::EventBus events_;
  BossPhaseSystem system_{events_};
  engine::Entity boss_{};
  std::vector<BossHealthChanged> healths_;
  std::vector<EnemyActionStarted> startedActions_;
};

}  // namespace rtype::game::testing
