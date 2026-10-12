#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "KeyBindings.hpp"
#include "PlayerAction.hpp"

using rtype::client::KeyBindings;
using rtype::client::PlayerAction;
using Key = sf::Keyboard::Key;

/**
 * Given the keys of the game
 * When each one is looked up
 * Then the arrows move, space shoots and any other key does nothing.
 */
TEST(KeyBindingsTest, ArrowsMoveSpaceShootsOtherKeysAreUnbound) {
  EXPECT_EQ(KeyBindings::actionFor(Key::Up), PlayerAction::Up);
  EXPECT_EQ(KeyBindings::actionFor(Key::Down), PlayerAction::Down);
  EXPECT_EQ(KeyBindings::actionFor(Key::Left), PlayerAction::Left);
  EXPECT_EQ(KeyBindings::actionFor(Key::Right), PlayerAction::Right);
  EXPECT_EQ(KeyBindings::actionFor(Key::Space), PlayerAction::Shoot);
  EXPECT_FALSE(KeyBindings::actionFor(Key::A).has_value());
  EXPECT_FALSE(KeyBindings::actionFor(Key::Unknown).has_value());
}
