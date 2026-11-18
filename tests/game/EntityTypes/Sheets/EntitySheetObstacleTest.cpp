#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetObstacleTestConstants.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "EntityType.hpp"
#include "MotionKind.hpp"
#include "ObstacleContact.hpp"

using rtype::engine::UnknownDataFileChoiceException;
using rtype::engine::UnknownDataFileKeyException;
using rtype::game::EntitySheet;
using rtype::game::EntityType;
using rtype::game::MotionKind;
using rtype::game::ObstacleContact;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::BLOCKING_OBSTACLE_TABLE;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::FATAL_OBSTACLE_TABLE;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::MISSILE_GAMEPLAY;
using rtype::game::testing::OBSTACLE_GAMEPLAY;
using rtype::game::testing::OBSTACLE_SIDE_RULE_TEXT;
using rtype::game::testing::OBSTACLE_TABLE_OF_AN_UNKNOWN_CONTACT;
using rtype::game::testing::OBSTACLE_TABLE_WITH_AN_UNKNOWN_KEY;
using rtype::game::testing::RIGHT_HEADING_TABLE;
using rtype::game::testing::SCROLL_MOTION;
using rtype::game::testing::SCROLL_MOTION_SIDE_RULE_TEXT;
using rtype::game::testing::SCROLL_MOTION_WITH_A_TOP_SPEED;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::STILL_HEADING_TABLE;
using ::testing::HasSubstr;

/**
 * Given the sheet of a wall that kills the ships that touch it
 * When its gameplay is read
 * Then it is an obstacle whose contact is fatal
 */
TEST_F(EntitySheetReaderFixture, ReadsAFatalObstacle) {
  const EntitySheet sheet =
      read({OBSTACLE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, STILL_HEADING_TABLE,
            FATAL_OBSTACLE_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.side, EntityType::Obstacle);
  EXPECT_EQ(sheet.obstacle.value_or(ObstacleContact::Blocking),
            ObstacleContact::Fatal);
}

/**
 * Given the sheet of a wall that stops the ships that touch it
 * When its gameplay is read
 * Then its contact is blocking
 */
TEST_F(EntitySheetReaderFixture, ReadsABlockingObstacle) {
  const EntitySheet sheet =
      read({OBSTACLE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, STILL_HEADING_TABLE,
            BLOCKING_OBSTACLE_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.obstacle.value_or(ObstacleContact::Fatal),
            ObstacleContact::Blocking);
}

/**
 * Given the sheet of a wall without an obstacle table
 * When its gameplay is read
 * Then it is refused: an obstacle says what it does to a ship
 */
TEST_F(EntitySheetReaderFixture, RefusesAnObstacleWithoutItsTable) {
  EXPECT_THAT(refusalOf({OBSTACLE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                         STILL_HEADING_TABLE, APPEARANCE}),
              HasSubstr(OBSTACLE_SIDE_RULE_TEXT));
}

/**
 * Given the sheet of a Bydo with an obstacle table
 * When its gameplay is read
 * Then it is refused: only an obstacle has one
 */
TEST_F(EntitySheetReaderFixture, RefusesAnObstacleTableOnABydo) {
  EXPECT_THAT(refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                         LEFT_HEADING_TABLE, FATAL_OBSTACLE_TABLE, APPEARANCE}),
              HasSubstr(OBSTACLE_SIDE_RULE_TEXT));
}

/**
 * Given the sheet of a wall whose contact is none of blocking and fatal
 * When its gameplay is read
 * Then it is refused for that value
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownContact) {
  EXPECT_THROW(
      static_cast<void>(read(
          {OBSTACLE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, STILL_HEADING_TABLE,
           OBSTACLE_TABLE_OF_AN_UNKNOWN_CONTACT, APPEARANCE})),
      UnknownDataFileChoiceException);
}

/**
 * Given the sheet of a wall whose obstacle table holds another key
 * When its gameplay is read
 * Then it is refused for that key
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownKeyOfTheObstacleTable) {
  EXPECT_THROW(
      static_cast<void>(read({OBSTACLE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                              STILL_HEADING_TABLE,
                              OBSTACLE_TABLE_WITH_AN_UNKNOWN_KEY, APPEARANCE})),
      UnknownDataFileKeyException);
}

/**
 * Given the sheet of a wall whose motion follows the scroll
 * When its gameplay is read
 * Then its motion is scroll, with no top speed of its own
 */
TEST_F(EntitySheetReaderFixture, ReadsAMotionThatFollowsTheScroll) {
  const EntitySheet sheet =
      read({OBSTACLE_GAMEPLAY, BYDO_HITBOX, SCROLL_MOTION, STILL_HEADING_TABLE,
            FATAL_OBSTACLE_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.motion.kind, MotionKind::Scroll);
  EXPECT_EQ(sheet.motion.topSpeed, 0.0F);
}

/**
 * Given the sheet of a wall whose motion follows the scroll and names a top
 * speed
 * When its gameplay is read
 * Then it is refused for that key, which nothing would read
 */
TEST_F(EntitySheetReaderFixture,
       RefusesATopSpeedOnAMotionThatFollowsTheScroll) {
  EXPECT_THROW(
      static_cast<void>(
          read({OBSTACLE_GAMEPLAY, BYDO_HITBOX, SCROLL_MOTION_WITH_A_TOP_SPEED,
                STILL_HEADING_TABLE, FATAL_OBSTACLE_TABLE, APPEARANCE})),
      UnknownDataFileKeyException);
}

/**
 * Given the sheet of a missile whose motion follows the scroll
 * When its gameplay is read
 * Then it is refused: only an enemy or an obstacle follows the scroll
 */
TEST_F(EntitySheetReaderFixture, RefusesAMissileThatFollowsTheScroll) {
  EXPECT_THAT(refusalOf({MISSILE_GAMEPLAY, BYDO_HITBOX, SCROLL_MOTION,
                         RIGHT_HEADING_TABLE, APPEARANCE}),
              HasSubstr(SCROLL_MOTION_SIDE_RULE_TEXT));
}
