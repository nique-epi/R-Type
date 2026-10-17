#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "BoundAction.hpp"
#include "KeyBindings.hpp"
#include "PlayerAction.hpp"

using rtype::client::BOUND_ACTIONS;
using rtype::client::BoundAction;
using rtype::client::KeyBindings;
using rtype::client::PlayerAction;
using Scan = sf::Keyboard::Scan;

/**
 * Given the keys of a first launch
 * When the arrows and space are read
 * Then the arrows move and space shoots
 */
TEST(KeyBindingsTest, AFirstLaunchMovesWithTheArrowsAndShootsWithSpace) {
  const KeyBindings keys;
  EXPECT_EQ(keys.playerActionFor(Scan::Up), PlayerAction::Up);
  EXPECT_EQ(keys.playerActionFor(Scan::Down), PlayerAction::Down);
  EXPECT_EQ(keys.playerActionFor(Scan::Left), PlayerAction::Left);
  EXPECT_EQ(keys.playerActionFor(Scan::Right), PlayerAction::Right);
  EXPECT_EQ(keys.playerActionFor(Scan::Space), PlayerAction::Shoot);
}

/**
 * Given the keys of a first launch
 * When R is read
 * Then it is ready, which no PlayerInput carries
 */
TEST(KeyBindingsTest, ReadyIsOnRAndMovesNothing) {
  const KeyBindings keys;
  EXPECT_EQ(keys.boundActionFor(Scan::R), BoundAction::Ready);
  EXPECT_FALSE(keys.playerActionFor(Scan::R).has_value());
}

/**
 * Given the keys of a first launch
 * When a key bound to nothing is read
 * Then it triggers nothing
 */
TEST(KeyBindingsTest, AnUnboundKeyTriggersNothing) {
  const KeyBindings keys;
  EXPECT_FALSE(keys.playerActionFor(Scan::A).has_value());
  EXPECT_FALSE(keys.playerActionFor(Scan::Unknown).has_value());
}

/**
 * Given shoot moved from space to Z
 * When Z and space are read
 * Then Z shoots and space does nothing any more
 */
TEST(KeyBindingsTest, AMovedActionFollowsItsNewKey) {
  KeyBindings keys;
  keys.bind(BoundAction::Shoot, Scan::Z);
  EXPECT_EQ(keys.playerActionFor(Scan::Z), PlayerAction::Shoot);
  EXPECT_FALSE(keys.playerActionFor(Scan::Space).has_value());
}

/**
 * Given ready moved onto space, the key of shoot
 * When each of the two actions asks who else has its key
 * Then each names the other
 */
TEST(KeyBindingsTest, TwoActionsOnOneKeyNameEachOther) {
  KeyBindings keys;
  keys.bind(BoundAction::Ready, Scan::Space);
  EXPECT_EQ(keys.otherActionWithKeyOf(BoundAction::Ready), BoundAction::Shoot);
  EXPECT_EQ(keys.otherActionWithKeyOf(BoundAction::Shoot), BoundAction::Ready);
}

/**
 * Given move up moved onto space, the key of shoot
 * When space is read in game
 * Then move up, listed first, takes it
 */
TEST(KeyBindingsTest, OnASharedKeyTheFirstActionListedWins) {
  KeyBindings keys;
  keys.bind(BoundAction::MoveUp, Scan::Space);
  EXPECT_EQ(keys.playerActionFor(Scan::Space), PlayerAction::Up);
}

/**
 * Given the keys of a first launch
 * When each action asks who else has its key
 * Then none shares its key
 */
TEST(KeyBindingsTest, AFirstLaunchSharesNoKey) {
  const KeyBindings keys;
  for (const BoundAction action : BOUND_ACTIONS) {
    EXPECT_FALSE(keys.otherActionWithKeyOf(action).has_value());
  }
}
