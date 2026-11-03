#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include "BoundAction.hpp"
#include "KeyBindings.hpp"
#include "PlayerCommands.hpp"

using rtype::client::BoundAction;
using rtype::client::KeyBindings;
using rtype::client::PlayerCommands;
using Scan = sf::Keyboard::Scan;

namespace {

constexpr std::uint8_t NOTHING_HELD = 0;
constexpr std::uint8_t UP_BIT = 0b00001;
constexpr std::uint8_t RIGHT_BIT = 0b01000;
constexpr std::uint8_t SHOOT_BIT = 0b10000;

}  // namespace

/**
 * Given a focused window
 * When up and space are pressed
 * Then both actions are held
 */
TEST(PlayerCommandsTest, PressedKeysAreHeld) {
  const KeyBindings keys;
  PlayerCommands commands(true, keys);
  commands.keyPressed(Scan::Up);
  commands.keyPressed(Scan::Space);
  EXPECT_EQ(commands.held().bits(), UP_BIT | SHOOT_BIT);
}

/**
 * Given a focused window with up and space held
 * When up is released
 * Then only shoot is held
 */
TEST(PlayerCommandsTest, ReleasedKeyIsNoLongerHeld) {
  const KeyBindings keys;
  PlayerCommands commands(true, keys);
  commands.keyPressed(Scan::Up);
  commands.keyPressed(Scan::Space);
  commands.keyReleased(Scan::Up);
  EXPECT_EQ(commands.held().bits(), SHOOT_BIT);
}

/**
 * Given a focused window with right held
 * When the same key is pressed again, as key repeat does
 * Then right is still the only action held
 */
TEST(PlayerCommandsTest, RepeatedPressChangesNothing) {
  const KeyBindings keys;
  PlayerCommands commands(true, keys);
  commands.keyPressed(Scan::Right);
  commands.keyPressed(Scan::Right);
  EXPECT_EQ(commands.held().bits(), RIGHT_BIT);
}

/**
 * Given a focused window with right held
 * When a key that is bound to no action is pressed
 * Then right is still the only action held
 */
TEST(PlayerCommandsTest, UnboundKeyChangesNothing) {
  const KeyBindings keys;
  PlayerCommands commands(true, keys);
  commands.keyPressed(Scan::Right);
  commands.keyPressed(Scan::A);
  EXPECT_EQ(commands.held().bits(), RIGHT_BIT);
}

/**
 * Given a focused window with left and space held
 * When the window loses the focus
 * Then nothing is held
 */
TEST(PlayerCommandsTest, LosingFocusReleasesEverything) {
  const KeyBindings keys;
  PlayerCommands commands(true, keys);
  commands.keyPressed(Scan::Left);
  commands.keyPressed(Scan::Space);
  commands.focusLost();
  EXPECT_EQ(commands.held().bits(), NOTHING_HELD);
}

/**
 * Given a window that lost the focus while left was held
 * When the focus comes back without any key pressed
 * Then nothing is held
 */
TEST(PlayerCommandsTest, RegainingFocusDoesNotRestoreHeldKeys) {
  const KeyBindings keys;
  PlayerCommands commands(true, keys);
  commands.keyPressed(Scan::Left);
  commands.focusLost();
  commands.keyReleased(Scan::Left);
  commands.focusGained();
  EXPECT_EQ(commands.held().bits(), NOTHING_HELD);
}

/**
 * Given a window without focus
 * When up is pressed
 * Then nothing is held
 */
TEST(PlayerCommandsTest, KeyPressedWithoutFocusIsIgnored) {
  const KeyBindings keys;
  PlayerCommands commands(false, keys);
  commands.keyPressed(Scan::Up);
  EXPECT_EQ(commands.held().bits(), NOTHING_HELD);
}

/**
 * Given a window that was without focus and just gained it
 * When up is pressed
 * Then up is held
 */
TEST(PlayerCommandsTest, KeyPressedAfterFocusGainedCounts) {
  const KeyBindings keys;
  PlayerCommands commands(false, keys);
  commands.focusGained();
  commands.keyPressed(Scan::Up);
  EXPECT_EQ(commands.held().bits(), UP_BIT);
}

/**
 * Given a focused window
 * When a key that was never pressed is released
 * Then nothing is held
 */
TEST(PlayerCommandsTest, ReleasingAKeyNeverPressedChangesNothing) {
  const KeyBindings keys;
  PlayerCommands commands(true, keys);
  commands.keyReleased(Scan::Down);
  EXPECT_EQ(commands.held().bits(), NOTHING_HELD);
}

/**
 * Given move up moved to W
 * When W is pressed
 * Then up is held
 */
TEST(PlayerCommandsTest, AMovedKeyHoldsItsAction) {
  KeyBindings keys;
  keys.bind(BoundAction::MoveUp, Scan::W);
  PlayerCommands commands(true, keys);
  commands.keyPressed(Scan::W);
  EXPECT_EQ(commands.held().bits(), UP_BIT);
}

/**
 * Given up and shoot held
 * When every action is released at once, as when the options open
 * Then nothing is held
 */
TEST(PlayerCommandsTest, ReleasingAllDropsEveryAction) {
  const KeyBindings keys;
  PlayerCommands commands(true, keys);
  commands.keyPressed(Scan::Up);
  commands.keyPressed(Scan::Space);
  commands.releaseAll();
  EXPECT_EQ(commands.held().bits(), 0U);
}
