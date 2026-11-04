#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "ActionKey.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyAppearance.hpp"
#include "EnemyAppearanceTestConstants.hpp"
#include "EnemyKind.hpp"
#include "EntityRegistry.hpp"
#include "EntitySpriteConstants.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"

using rtype::client::appearanceOf;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::showEnemyAction;
using rtype::client::SpriteAnimation;
using rtype::client::startedActionOf;
using rtype::client::testing::SCYTHE_ATTACK;
using rtype::client::testing::TEST_APPEARANCE;
using rtype::client::testing::UNKNOWN_ATTACK;
using rtype::client::testing::WIDE_ANCHOR;
using rtype::client::testing::WIDE_FRAME;
using rtype::game::ActionKey;
using rtype::game::EnemyKind;
using rtype::game::EnemyPhase;

/**
 * Given every kind of enemy
 * When its appearance is asked for
 * Then it is drawn from the entity sheet with the Bydo idle and no attack
 * animation, as long as no pull request fills its row
 */
TEST(EnemyAppearance, EveryKindIsDrawnAsABydoForNow) {
  for (const EnemyKind kind : rtype::game::ALL_ENEMY_KINDS) {
    SCOPED_TRACE(static_cast<int>(kind));

    EXPECT_EQ(std::string{appearanceOf(kind).sheetId}, ENTITY_SHEET_ID);
    EXPECT_TRUE(appearanceOf(kind).actions.empty());
    EXPECT_EQ(appearanceOf(kind).idle.frames.front(),
              rtype::client::BYDO_FRAME);
  }
}

/**
 * Given an appearance with a held Signal and a Recover played once
 * When the Recover of the attack is started
 * Then its animation starts at zero with its anchor, and the idle follows it
 */
TEST(EnemyAppearance, AnActionPlayedOnceIsFollowedByTheIdle) {
  const std::optional<SpriteAnimation> started = startedActionOf(
      TEST_APPEARANCE,
      ActionKey{.attack = SCYTHE_ATTACK, .phase = EnemyPhase::Recover});

  ASSERT_TRUE(started.has_value());
  const SpriteAnimation found = started.value_or(SpriteAnimation{});
  EXPECT_EQ(found.animation.anchor, WIDE_ANCHOR);
  EXPECT_TRUE(found.afterwards.has_value());
}

/**
 * Given an appearance with a held Signal
 * When the Signal of the attack is started
 * Then nothing follows it: it stays until the next message
 */
TEST(EnemyAppearance, AHeldActionIsFollowedByNothing) {
  const std::optional<SpriteAnimation> started = startedActionOf(
      TEST_APPEARANCE,
      ActionKey{.attack = SCYTHE_ATTACK, .phase = EnemyPhase::Signal});

  ASSERT_TRUE(started.has_value());
  EXPECT_FALSE(started.value_or(SpriteAnimation{}).afterwards.has_value());
}

/**
 * Given an appearance with two actions of one attack
 * When an attack it has none for, and a phase it has none for, are started
 * Then nothing starts
 */
TEST(EnemyAppearance, AnActionWithoutAnimationStartsNothing) {
  EXPECT_FALSE(
      startedActionOf(TEST_APPEARANCE, ActionKey{.attack = UNKNOWN_ATTACK,
                                                 .phase = EnemyPhase::Signal})
          .has_value());
  EXPECT_FALSE(
      startedActionOf(TEST_APPEARANCE, ActionKey{.attack = SCYTHE_ATTACK,
                                                 .phase = EnemyPhase::Strike})
          .has_value());
}

/**
 * Given an enemy of a kind drawn as a Bydo, which has no action animation
 * When an action is shown on it
 * Then its animation and its frame stay those it had
 */
TEST(EnemyAppearance, ShowingAnActionTheKindHasNoAnimationForChangesNothing) {
  rtype::engine::EntityRegistry entities;
  rtype::engine::ComponentRegistry components{entities};
  const rtype::engine::Entity enemy = entities.create();
  components.add(enemy, EnemyKind::Reaper);
  components.add(enemy,
                 rtype::game::Sprite{.assetId = ENTITY_SHEET_ID,
                                     .frame = WIDE_FRAME,
                                     .layer = rtype::game::Layer::Enemies});
  components.add(enemy, SpriteAnimation{});

  showEnemyAction(
      components, enemy,
      ActionKey{.attack = SCYTHE_ATTACK, .phase = EnemyPhase::Strike});

  EXPECT_EQ(components.get<rtype::game::Sprite>(enemy)->frame, WIDE_FRAME);
  EXPECT_FALSE(components.get<SpriteAnimation>(enemy)->afterwards.has_value());
}
