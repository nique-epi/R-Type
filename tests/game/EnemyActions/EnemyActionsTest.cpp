#include <gtest/gtest.h>
#include <vector>
#include "ActionKey.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStarted.hpp"
#include "EnemyActions.hpp"
#include "EnemyActionsFixture.hpp"
#include "EnemyPhase.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"

using rtype::game::ActionKey;
using rtype::game::CurrentAction;
using rtype::game::EnemyActionStart;
using rtype::game::EnemyActionStarted;
using rtype::game::EnemyPhase;
using rtype::game::testing::EnemyActionsFixture;

/**
 * Given an enemy that is idle
 * When it enters the phase Signal of attack 1
 * Then its CurrentAction holds that action and one EnemyActionStarted naming
 * it is delivered
 */
TEST_F(EnemyActionsFixture, StartingAnActionRecordsItAndPublishesIt) {
  const ActionKey signal{.attack = 1, .phase = EnemyPhase::Signal};

  rtype::game::startEnemyAction(
      events(), components(),
      EnemyActionStart{.enemy = enemy(), .action = signal});

  const CurrentAction* const current = components().get<CurrentAction>(enemy());
  ASSERT_NE(current, nullptr);
  EXPECT_EQ(current->key, signal);
  EXPECT_EQ(delivered(),
            (std::vector<EnemyActionStarted>{{.networkId = enemyNetworkId(),
                                              .attack = 1,
                                              .phase = EnemyPhase::Signal}}));
}

/**
 * Given an enemy in the phase Signal of attack 1
 * When it enters the phase Strike, then ends its action
 * Then its action was Strike, then none, and exactly two events were
 * published: nothing is published for the end
 */
TEST_F(EnemyActionsFixture,
       AnActionReplacesTheLastOneAndEndingItPublishesNothing) {
  rtype::game::startEnemyAction(
      events(), components(),
      EnemyActionStart{.enemy = enemy(),
                       .action = {.attack = 1, .phase = EnemyPhase::Signal}});
  rtype::game::startEnemyAction(
      events(), components(),
      EnemyActionStart{.enemy = enemy(),
                       .action = {.attack = 1, .phase = EnemyPhase::Strike}});
  const ActionKey during =
      components().get<CurrentAction>(enemy())->key.value_or(ActionKey{});

  rtype::game::endEnemyAction(components(), enemy());

  EXPECT_EQ(during.phase, EnemyPhase::Strike);
  EXPECT_FALSE(components().get<CurrentAction>(enemy())->key.has_value());
  EXPECT_EQ(delivered().size(), 2U);
}

/**
 * Given an entity that has no CurrentAction and no NetworkIdentity
 * When an action is started on it
 * Then nothing is published
 */
TEST_F(EnemyActionsFixture, AnEntityThatIsNoEnemyPublishesNothing) {
  rtype::engine::EntityRegistry stranger;
  rtype::engine::ComponentRegistry strangerComponents{stranger};
  const rtype::engine::Entity entity = stranger.create();

  rtype::game::startEnemyAction(
      events(), strangerComponents,
      EnemyActionStart{.enemy = entity,
                       .action = {.attack = 1, .phase = EnemyPhase::Signal}});

  EXPECT_TRUE(delivered().empty());
}

/**
 * Given an enemy
 * When an action is started on it from inside a forEach of the registry
 * Then it is recorded and published without the registry refusing
 */
TEST_F(EnemyActionsFixture, AnActionCanStartInsideAForEach) {
  components().forEach<CurrentAction, CurrentAction>(
      [this](rtype::engine::Entity entity, CurrentAction&,
             const CurrentAction&) {
        rtype::game::startEnemyAction(
            events(), components(),
            EnemyActionStart{
                .enemy = entity,
                .action = {.attack = 3, .phase = EnemyPhase::Recover}});
      });

  EXPECT_EQ(delivered().size(), 1U);
  EXPECT_EQ(components().get<CurrentAction>(enemy())->key,
            (ActionKey{.attack = 3, .phase = EnemyPhase::Recover}));
}
