#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "OwnMissilesTestConstants.hpp"
#include "PredictedCommand.hpp"
#include "PredictedGun.hpp"
#include "PredictedGunFixtures.hpp"
#include "PredictedGunTestConstants.hpp"

using rtype::client::PredictedCommand;
using rtype::client::PredictedGun;
using rtype::client::testing::ACKNOWLEDGED_BEFORE_THE_SHIP;
using rtype::client::testing::COMMANDS_OF_THREE_SHOTS;
using rtype::client::testing::commandsHoldingShoot;
using rtype::client::testing::FIRST_SHOT_OF_THE_SHIP;
using rtype::client::testing::FIVE_COMMANDS;
using rtype::client::testing::INPUT_THE_SERVER_FIRED;
using rtype::client::testing::inputsFiredBy;
using rtype::client::testing::NOTHING_ACKNOWLEDGED;
using rtype::client::testing::READY_GUN;
using rtype::client::testing::SECOND_SHOT_INPUT;
using rtype::client::testing::SHOT_AFTER_THE_SERVER_ONE;
using rtype::client::testing::THIRD_SHOT_INPUT;
using rtype::client::testing::THIRTIETH_INPUT;
using rtype::client::testing::THIRTY_COMMANDS;

/**
 * Given the gun of the shipped ship, ready at once
 * When 41 commands hold shoot
 * Then it fires on the first, the 21st and the 41st, every 20 ticks, as
 * WeaponSystem does
 */
TEST(PredictedGun, FiresAsTheServerDoes) {
  PredictedGun gun;
  static_cast<void>(gun.arm(READY_GUN, NOTHING_ACKNOWLEDGED));

  EXPECT_EQ(
      inputsFiredBy(gun, commandsHoldingShoot(COMMANDS_OF_THREE_SHOTS)),
      (std::vector<std::uint32_t>{1, SECOND_SHOT_INPUT, THIRD_SHOT_INPUT}));
}

/**
 * Given a gun that fired on command 1, then kept commands up to 30 holding
 * shoot
 * When the server says it fired on command 3
 * Then playing the commands after 3 again fires on command 23 only
 */
TEST(PredictedGun, PlaysTheCommandsAfterAShotOfTheServerAgain) {
  PredictedGun gun;
  static_cast<void>(gun.arm(READY_GUN, NOTHING_ACKNOWLEDGED));
  static_cast<void>(inputsFiredBy(gun, commandsHoldingShoot(THIRTY_COMMANDS)));

  const std::vector<PredictedCommand> firing =
      gun.confirm(INPUT_THE_SERVER_FIRED);

  ASSERT_EQ(firing.size(), 1U);
  EXPECT_EQ(firing.front().inputNumber, SHOT_AFTER_THE_SERVER_ONE);
}

/**
 * Given a gun whose commands up to 30 were confirmed by the server
 * When the server says it fired on command 3
 * Then nothing is played again: the commands it forgot are not kept
 */
TEST(PredictedGun, ForgetsTheCommandsTheServerConfirmed) {
  PredictedGun gun;
  static_cast<void>(gun.arm(READY_GUN, NOTHING_ACKNOWLEDGED));
  static_cast<void>(inputsFiredBy(gun, commandsHoldingShoot(THIRTY_COMMANDS)));

  gun.forgetUpTo(THIRTIETH_INPUT);

  EXPECT_TRUE(gun.confirm(INPUT_THE_SERVER_FIRED).empty());
}

/**
 * Given commands 1 to 5 holding shoot, kept while the player had no ship
 * When the gun is armed for a ship whose state came once the server applied
 * input 2
 * Then playing them fires on command 3, the first the server applies to the
 * new ship
 */
TEST(PredictedGun, ArmingPlaysTheCommandsSentBeforeTheShip) {
  PredictedGun gun;
  for (const PredictedCommand& command : commandsHoldingShoot(FIVE_COMMANDS)) {
    gun.record(command);
  }

  const std::vector<PredictedCommand> firing =
      gun.arm(READY_GUN, ACKNOWLEDGED_BEFORE_THE_SHIP);

  ASSERT_EQ(firing.size(), 1U);
  EXPECT_EQ(firing.front().inputNumber, FIRST_SHOT_OF_THE_SHIP);
}
