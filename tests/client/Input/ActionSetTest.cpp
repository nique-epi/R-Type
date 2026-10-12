#include <gtest/gtest.h>
#include <array>
#include <cstdint>
#include "ActionSet.hpp"
#include "PlayerAction.hpp"

using rtype::client::ActionSet;
using rtype::client::PlayerAction;

namespace {

constexpr std::uint8_t NOTHING_HELD = 0;
constexpr std::uint8_t UP_BIT = 0b00001;
constexpr std::uint8_t DOWN_BIT = 0b00010;
constexpr std::uint8_t LEFT_BIT = 0b00100;
constexpr std::uint8_t RIGHT_BIT = 0b01000;
constexpr std::uint8_t SHOOT_BIT = 0b10000;
constexpr std::uint8_t ALL_ACTIONS = 0b11111;

struct ActionBit {
  PlayerAction action;
  std::uint8_t bit;
};

}  // namespace

/**
 * Given nothing held
 * When each action is added alone
 * Then the byte has the bit the protocol gives it: 0 up to 4 shoot.
 */
TEST(ActionSetTest, EachActionSetsItsProtocolBit) {
  const std::array cases{
      ActionBit{.action = PlayerAction::Up, .bit = UP_BIT},
      ActionBit{.action = PlayerAction::Down, .bit = DOWN_BIT},
      ActionBit{.action = PlayerAction::Left, .bit = LEFT_BIT},
      ActionBit{.action = PlayerAction::Right, .bit = RIGHT_BIT},
      ActionBit{.action = PlayerAction::Shoot, .bit = SHOOT_BIT}};
  for (const auto& entry : cases) {
    ActionSet set;
    set.add(entry.action);
    EXPECT_EQ(set.bits(), entry.bit);
    EXPECT_TRUE(set.has(entry.action));
  }
}

/**
 * Given a set with nothing added
 * When the byte is read
 * Then it is 0, so a PlayerInput can be sent with no key held
 */
TEST(ActionSetTest, EmptySetIsZero) {
  const ActionSet set;
  EXPECT_EQ(set.bits(), NOTHING_HELD);
}

/**
 * Given a set holding every action
 * When the byte is read
 * Then it is 0b11111: bits 5 to 7 are never set
 */
TEST(ActionSetTest, FullSetLeavesTheHighBitsClear) {
  ActionSet set;
  set.add(PlayerAction::Up);
  set.add(PlayerAction::Down);
  set.add(PlayerAction::Left);
  set.add(PlayerAction::Right);
  set.add(PlayerAction::Shoot);
  EXPECT_EQ(set.bits(), ALL_ACTIONS);
}

/**
 * Given a set holding up and shoot
 * When up is removed
 * Then only shoot stays
 */
TEST(ActionSetTest, RemovingAnActionKeepsTheOthers) {
  ActionSet set;
  set.add(PlayerAction::Up);
  set.add(PlayerAction::Shoot);
  set.remove(PlayerAction::Up);
  EXPECT_EQ(set.bits(), SHOOT_BIT);
}

/**
 * Given a set holding only shoot
 * When up, which is not held, is removed
 * Then shoot is still the only action held
 */
TEST(ActionSetTest, RemovingAnActionNotHeldChangesNothing) {
  ActionSet set;
  set.add(PlayerAction::Shoot);
  set.remove(PlayerAction::Up);
  EXPECT_EQ(set.bits(), SHOOT_BIT);
}
