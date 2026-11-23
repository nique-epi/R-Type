#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include "EntitySpawned.hpp"
#include "FixedEntityTypeSourceConstants.hpp"
#include "OwnMissileWeaponsTestConstants.hpp"
#include "OwnMissilesFixture.hpp"
#include "OwnMissilesTestConstants.hpp"
#include "Position.hpp"

using rtype::client::testing::FAN_MISSILES;
using rtype::client::testing::FAN_OFFSET_AFTER_THREE_TICKS;
using rtype::client::testing::FAN_WEAPON;
using rtype::client::testing::FIRING_INPUT;
using rtype::client::testing::MISSILES_OF_TWO_SHOTS_OF_ROWS;
using rtype::client::testing::MUZZLE_ROW;
using rtype::client::testing::NOTHING_ACKNOWLEDGED;
using rtype::client::testing::OwnMissilesFixture;
using rtype::client::testing::PLAYER_MISSILE_TYPE_NUMBER;
using rtype::client::testing::POSITION_TOLERANCE;
using rtype::client::testing::ROWS_WEAPON;
using rtype::client::testing::SERVER_MISSILE_ID;
using rtype::client::testing::SERVER_START_OF_THE_UPPER_MISSILE;
using rtype::client::testing::SHIP_WHEN_IT_FIRES;
using rtype::client::testing::SHOOTING_PLAYER;
using rtype::client::testing::THREE_TICKS_LATER_INPUT;
using rtype::client::testing::TWENTIETH_COMMAND;
using rtype::client::testing::UPPER_MISSILE_RANK;
using rtype::network::EntitySpawned;

/**
 * Given the missiles of a player whose ship holds the fan of three
 * When a command holding shoot is sent
 * Then the three missiles of the fan are drawn at once
 */
TEST_F(OwnMissilesFixture, AFanFiresEachOfItsMissiles) {
  missiles().acknowledge(NOTHING_ACKNOWLEDGED, FAN_WEAPON);

  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  EXPECT_EQ(drawnMissiles().size(), FAN_MISSILES);
}

/**
 * Given a fan of three fired by input 1 from a ship on row 300
 * When the commands up to input 4 are sent
 * Then its middle missile is still on row 300, and the two others are 64 sin
 * 12 degrees, 13.306 units, above and below it: each flies along its heading
 */
TEST_F(OwnMissilesFixture, TheMissilesOfAFanFlyAlongTheirHeadings) {
  missiles().acknowledge(NOTHING_ACKNOWLEDGED, FAN_WEAPON);
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  holdNothingUpTo(THREE_TICKS_LATER_INPUT);

  std::vector<rtype::game::Position> drawn = drawnMissiles();
  ASSERT_EQ(drawn.size(), FAN_MISSILES);
  std::ranges::sort(drawn, {}, &rtype::game::Position::y);
  EXPECT_NEAR(drawn.at(0).y, MUZZLE_ROW - FAN_OFFSET_AFTER_THREE_TICKS,
              POSITION_TOLERANCE);
  EXPECT_NEAR(drawn.at(1).y, MUZZLE_ROW, POSITION_TOLERANCE);
  EXPECT_NEAR(drawn.at(2).y, MUZZLE_ROW + FAN_OFFSET_AFTER_THREE_TICKS,
              POSITION_TOLERANCE);
}

/**
 * Given a fan of three fired by input 1
 * When the server tells it fired the upper missile, the third of the shot,
 * 4 units ahead of where the client laid it
 * Then that missile moves to the start of the server, and the lower one stays
 * below the muzzle: the missile of the server stands for the one of its rank
 */
TEST_F(OwnMissilesFixture, AMissileOfTheServerStandsForTheOneOfItsRank) {
  missiles().acknowledge(NOTHING_ACKNOWLEDGED, FAN_WEAPON);
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  missiles().ownMissileSpawned(
      EntitySpawned{.entityId = SERVER_MISSILE_ID,
                    .entityType = PLAYER_MISSILE_TYPE_NUMBER,
                    .x = SERVER_START_OF_THE_UPPER_MISSILE.x,
                    .y = SERVER_START_OF_THE_UPPER_MISSILE.y,
                    .owner = SHOOTING_PLAYER,
                    .firingInput = FIRING_INPUT,
                    .shotRank = UPPER_MISSILE_RANK});

  std::vector<rtype::game::Position> drawn = drawnMissiles();
  ASSERT_EQ(drawn.size(), FAN_MISSILES);
  std::ranges::sort(drawn, {}, &rtype::game::Position::y);
  EXPECT_NEAR(drawn.at(0).x, SERVER_START_OF_THE_UPPER_MISSILE.x,
              POSITION_TOLERANCE);
  EXPECT_GT(drawn.at(2).y, MUZZLE_ROW);
}

/**
 * Given the missiles of a player whose ship holds the rows, which fire every
 * 10 ticks where the gun of the sheet fires every 20
 * When commands holding shoot are sent up to input 20
 * Then two shots of two missiles are drawn: the gun follows the weapon held
 */
TEST_F(OwnMissilesFixture, TheGunFollowsTheWeaponHeld) {
  missiles().acknowledge(NOTHING_ACKNOWLEDGED, ROWS_WEAPON);

  holdShootUpTo(TWENTIETH_COMMAND);

  EXPECT_EQ(drawnMissiles().size(), MISSILES_OF_TWO_SHOTS_OF_ROWS);
}

/**
 * Given the missiles of a player whose ship held the fan
 * When a state lists a new ship of the player, which fires again
 * Then one missile is drawn: a new ship holds the gun of its sheet
 */
TEST_F(OwnMissilesFixture, ANewShipFiresTheGunOfItsSheet) {
  missiles().acknowledge(NOTHING_ACKNOWLEDGED, FAN_WEAPON);
  missiles().restart(NOTHING_ACKNOWLEDGED, SHIP_WHEN_IT_FIRES);

  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  EXPECT_EQ(drawnMissiles().size(), 1U);
}
