#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include "PlayerCommands.hpp"

using rtype::client::PlayerCommands;
using Key = sf::Keyboard::Key;

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
  PlayerCommands commands(true);
  commands.keyPressed(Key::Up);
  commands.keyPressed(Key::Space);
  EXPECT_EQ(commands.held().bits(), UP_BIT | SHOOT_BIT);
}

/**
 * Given a focused window with up and space held
 * When up is released
 * Then only shoot is held
 */
TEST(PlayerCommandsTest, ReleasedKeyIsNoLongerHeld) {
  PlayerCommands commands(true);
  commands.keyPressed(Key::Up);
  commands.keyPressed(Key::Space);
  commands.keyReleased(Key::Up);
  EXPECT_EQ(commands.held().bits(), SHOOT_BIT);
}

/**
 * Given a focused window with right held
 * When the same key is pressed again, as key repeat does
 * Then right is still the only action held
 */
TEST(PlayerCommandsTest, RepeatedPressChangesNothing) {
  PlayerCommands commands(true);
  commands.keyPressed(Key::Right);
  commands.keyPressed(Key::Right);
  EXPECT_EQ(commands.held().bits(), RIGHT_BIT);
}

/**
 * Given a focused window with right held
 * When a key that is bound to no action is pressed
 * Then right is still the only action held
 */
TEST(PlayerCommandsTest, UnboundKeyChangesNothing) {
  PlayerCommands commands(true);
  commands.keyPressed(Key::Right);
  commands.keyPressed(Key::A);
  EXPECT_EQ(commands.held().bits(), RIGHT_BIT);
}

/**
 * Given a focused window with left and space held
 * When the window loses the focus
 * Then nothing is held
 */
TEST(PlayerCommandsTest, LosingFocusReleasesEverything) {
  PlayerCommands commands(true);
  commands.keyPressed(Key::Left);
  commands.keyPressed(Key::Space);
  commands.focusLost();
  EXPECT_EQ(commands.held().bits(), NOTHING_HELD);
}

/**
 * Given a window that lost the focus while left was held
 * When the focus comes back without any key pressed
 * Then nothing is held
 */
TEST(PlayerCommandsTest, RegainingFocusDoesNotRestoreHeldKeys) {
  PlayerCommands commands(true);
  commands.keyPressed(Key::Left);
  commands.focusLost();
  commands.keyReleased(Key::Left);
  commands.focusGained();
  EXPECT_EQ(commands.held().bits(), NOTHING_HELD);
}

/**
 * Given a window without focus
 * When up is pressed
 * Then nothing is held
 */
TEST(PlayerCommandsTest, KeyPressedWithoutFocusIsIgnored) {
  PlayerCommands commands(false);
  commands.keyPressed(Key::Up);
  EXPECT_EQ(commands.held().bits(), NOTHING_HELD);
}

/**
 * Given a window that was without focus and just gained it
 * When up is pressed
 * Then up is held
 */
TEST(PlayerCommandsTest, KeyPressedAfterFocusGainedCounts) {
  PlayerCommands commands(false);
  commands.focusGained();
  commands.keyPressed(Key::Up);
  EXPECT_EQ(commands.held().bits(), UP_BIT);
}

/**
 * Given a focused window
 * When a key that was never pressed is released
 * Then nothing is held
 */
TEST(PlayerCommandsTest, ReleasingAKeyNeverPressedChangesNothing) {
  PlayerCommands commands(true);
  commands.keyReleased(Key::Down);
  EXPECT_EQ(commands.held().bits(), NOTHING_HELD);
}
