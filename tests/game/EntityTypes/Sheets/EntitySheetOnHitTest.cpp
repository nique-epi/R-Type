#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetObstacleTestConstants.hpp"
#include "EntitySheetOnHitFixture.hpp"
#include "EntitySheetOnHitTestConstants.hpp"
#include "EntitySheetReaderTestConstants.hpp"

using rtype::engine::UnknownDataFileChoiceException;
using rtype::game::EntitySheet;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::EntitySheetOnHitFixture;
using rtype::game::testing::FATAL_OBSTACLE_TABLE;
using rtype::game::testing::OBSTACLE_GAMEPLAY;
using rtype::game::testing::ON_HIT_SIDE_RULE_TEXT;
using rtype::game::testing::SHIP_GAMEPLAY_NAMING_THE_HANDLER;
using rtype::game::testing::SHIP_HITBOX;
using rtype::game::testing::SHIP_MOTION;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::STILL_HEADING_TABLE;
using rtype::game::testing::WALL_GAMEPLAY_NAMING_AN_UNKNOWN_HANDLER;
using rtype::game::testing::WALL_GAMEPLAY_NAMING_THE_HANDLER;
using ::testing::HasSubstr;

/**
 * Given the sheet of a wall whose on_hit names the handler `crack`
 * When its gameplay is read
 * Then it holds that handler
 */
TEST_F(EntitySheetOnHitFixture, ReadsTheHandlerASheetNames) {
  const EntitySheet sheet =
      read({WALL_GAMEPLAY_NAMING_THE_HANDLER, BYDO_HITBOX, STEADY_MOTION,
            STILL_HEADING_TABLE, FATAL_OBSTACLE_TABLE, APPEARANCE});

  ASSERT_NE(sheet.onHit, nullptr);
  EXPECT_EQ(sheet.onHit, handler());
}

/**
 * Given the sheet of a wall whose on_hit names a handler no script declared
 * When its gameplay is read
 * Then it is refused for that value
 */
TEST_F(EntitySheetOnHitFixture, RefusesAnUnknownHandler) {
  EXPECT_THROW(
      static_cast<void>(read({WALL_GAMEPLAY_NAMING_AN_UNKNOWN_HANDLER,
                              BYDO_HITBOX, STEADY_MOTION, STILL_HEADING_TABLE,
                              FATAL_OBSTACLE_TABLE, APPEARANCE})),
      UnknownDataFileChoiceException);
}

/**
 * Given the sheet of a ship whose on_hit names the handler `crack`
 * When its gameplay is read
 * Then it is refused: a ship answers a hit with its lives
 */
TEST_F(EntitySheetOnHitFixture, RefusesAnOnHitOnAShip) {
  EXPECT_THAT(refusalOf({SHIP_GAMEPLAY_NAMING_THE_HANDLER, SHIP_HITBOX,
                         SHIP_MOTION, APPEARANCE}),
              HasSubstr(ON_HIT_SIDE_RULE_TEXT));
}

/**
 * Given the sheet of a wall without on_hit
 * When its gameplay is read
 * Then it holds no handler
 */
TEST_F(EntitySheetOnHitFixture, ASheetWithoutOnHitHoldsNoHandler) {
  const EntitySheet sheet =
      read({OBSTACLE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, STILL_HEADING_TABLE,
            FATAL_OBSTACLE_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.onHit, nullptr);
}
