#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "ActionRequestingBehaviour.hpp"
#include "BehaviourView.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStarted.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "PilotSystemActionTestConstants.hpp"
#include "PilotSystemFixture.hpp"
#include "TimeConstants.hpp"
#include "ViewRecordingBehaviour.hpp"

using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::BehaviourView;
using rtype::game::CurrentAction;
using rtype::game::EnemyActionStarted;
using rtype::game::Intent;
using rtype::game::testing::A_SECOND_IN_ITS_PHASE;
using rtype::game::testing::ActionRequestingBehaviour;
using rtype::game::testing::CLOCK_TICKS;
using rtype::game::testing::PilotSystemFixture;
using rtype::game::testing::SIGNAL_OF_ATTACK_FOUR;
using rtype::game::testing::SOME_ENEMY_NETWORK_ID;
using rtype::game::testing::STRIKE_OF_ATTACK_THREE;
using rtype::game::testing::ViewRecordingBehaviour;

/**
 * Given an enemy whose behaviour starts the strike of attack 3
 * When the pilots run once
 * Then the enemy is in the strike of attack 3
 */
TEST_F(PilotSystemFixture, AStartedActionIsRecordedOnTheEnemy) {
  const ActionRequestingBehaviour behaviour{
      Intent{.startsAction = STRIKE_OF_ATTACK_THREE}};
  const Entity enemy = addEnemy(behaviour, SOME_ENEMY_NETWORK_ID);

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(actionOf(enemy), STRIKE_OF_ATTACK_THREE);
}

/**
 * Given an enemy whose behaviour starts the strike of attack 3
 * When the pilots run once and the events are delivered
 * Then one EnemyActionStarted names its network id, attack 3 and the strike
 */
TEST_F(PilotSystemFixture, AStartedActionIsPublishedWithTheIdOfTheEnemy) {
  const ActionRequestingBehaviour behaviour{
      Intent{.startsAction = STRIKE_OF_ATTACK_THREE}};
  static_cast<void>(addEnemy(behaviour, SOME_ENEMY_NETWORK_ID));

  run(SIMULATION_TICK_DURATION);

  const std::vector<EnemyActionStarted> expected{
      {.networkId = SOME_ENEMY_NETWORK_ID,
       .attack = STRIKE_OF_ATTACK_THREE.attack,
       .phase = STRIKE_OF_ATTACK_THREE.phase}};
  EXPECT_EQ(takeStartedActions(), expected);
}

/**
 * Given an enemy in the strike of attack 3 whose behaviour ends its action
 * When the pilots run once
 * Then the enemy is idle and nothing is published
 */
TEST_F(PilotSystemFixture, AnEndedActionLeavesTheEnemyIdle) {
  const ActionRequestingBehaviour behaviour{Intent{.endsAction = true}};
  const Entity enemy = addEnemy(behaviour, SOME_ENEMY_NETWORK_ID);
  components().get<CurrentAction>(enemy)->key = STRIKE_OF_ATTACK_THREE;

  run(SIMULATION_TICK_DURATION);

  EXPECT_FALSE(actionOf(enemy).has_value());
  EXPECT_TRUE(takeStartedActions().empty());
}

/**
 * Given an enemy in the strike of attack 3 whose behaviour ends that action
 * and starts the signal of attack 4 at the same tick
 * When the pilots run once
 * Then the enemy is in the signal of attack 4, announced once
 */
TEST_F(PilotSystemFixture, AnActionEndedAndStartedInOneTickIsTheNewOne) {
  const ActionRequestingBehaviour behaviour{
      Intent{.startsAction = SIGNAL_OF_ATTACK_FOUR, .endsAction = true}};
  const Entity enemy = addEnemy(behaviour, SOME_ENEMY_NETWORK_ID);
  components().get<CurrentAction>(enemy)->key = STRIKE_OF_ATTACK_THREE;

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(actionOf(enemy), SIGNAL_OF_ATTACK_FOUR);
  EXPECT_EQ(takeStartedActions().size(), 1U);
}

/**
 * Given an enemy in the strike of attack 3 whose behaviour asks nothing
 * When the pilots run once
 * Then the enemy is still in the strike and nothing is published
 */
TEST_F(PilotSystemFixture, ABehaviourThatAsksNothingLeavesTheActionAlone) {
  const ActionRequestingBehaviour behaviour{Intent{}};
  const Entity enemy = addEnemy(behaviour, SOME_ENEMY_NETWORK_ID);
  components().get<CurrentAction>(enemy)->key = STRIKE_OF_ATTACK_THREE;

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(actionOf(enemy), STRIKE_OF_ATTACK_THREE);
  EXPECT_TRUE(takeStartedActions().empty());
}

/**
 * Given a body without a CurrentAction whose behaviour starts an action
 * When the pilots run once
 * Then nothing is published
 */
TEST_F(PilotSystemFixture, ABodyWithoutACurrentActionAnnouncesNothing) {
  const ActionRequestingBehaviour behaviour{
      Intent{.startsAction = STRIKE_OF_ATTACK_THREE}};
  const Entity body = addEnemy(behaviour, SOME_ENEMY_NETWORK_ID);
  components().remove<CurrentAction>(body);

  run(SIMULATION_TICK_DURATION);

  EXPECT_TRUE(takeStartedActions().empty());
}

/**
 * Given an enemy that just entered the strike of attack 3
 * When the pilots run three ticks
 * Then its behaviour sees the strike of attack 3, three ticks old
 */
TEST_F(PilotSystemFixture, ABehaviourSeesItsActionAndHowLongItHasLasted) {
  std::optional<BehaviourView> seen;
  const ViewRecordingBehaviour behaviour{seen};
  const Entity enemy = addEnemy(behaviour, SOME_ENEMY_NETWORK_ID);
  components().get<CurrentAction>(enemy)->key = STRIKE_OF_ATTACK_THREE;

  for (int tick = 0; tick < CLOCK_TICKS; ++tick) {
    run(SIMULATION_TICK_DURATION);
  }

  ASSERT_TRUE(seen.has_value());
  const BehaviourView view = seen.value_or(BehaviourView{});
  EXPECT_EQ(view.action, STRIKE_OF_ATTACK_THREE);
  EXPECT_EQ(view.actionAge, CLOCK_TICKS * SIMULATION_TICK_DURATION);
}

/**
 * Given an enemy idle for a second whose behaviour starts the signal of
 * attack 4
 * When the pilots run once
 * Then the clock of its action is back at zero
 */
TEST_F(PilotSystemFixture, AStartedActionStartsItsClockAtZero) {
  const ActionRequestingBehaviour behaviour{
      Intent{.startsAction = SIGNAL_OF_ATTACK_FOUR}};
  const Entity enemy = addEnemy(behaviour, SOME_ENEMY_NETWORK_ID);
  components().get<CurrentAction>(enemy)->since = A_SECOND_IN_ITS_PHASE;

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(components().get<CurrentAction>(enemy)->since, Duration::zero());
}

/**
 * Given an enemy a second into the strike of attack 3 whose behaviour ends it
 * When the pilots run once
 * Then the clock of its idle time starts at zero
 */
TEST_F(PilotSystemFixture, AnEndedActionStartsTheIdleClockAtZero) {
  const ActionRequestingBehaviour behaviour{Intent{.endsAction = true}};
  const Entity enemy = addEnemy(behaviour, SOME_ENEMY_NETWORK_ID);
  auto* const current = components().get<CurrentAction>(enemy);
  current->key = STRIKE_OF_ATTACK_THREE;
  current->since = A_SECOND_IN_ITS_PHASE;

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(components().get<CurrentAction>(enemy)->since, Duration::zero());
}
