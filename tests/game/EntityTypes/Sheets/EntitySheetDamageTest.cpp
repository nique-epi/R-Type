#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetDamageTestConstants.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"

using rtype::engine::DataFileValueOutOfRangeException;
using rtype::game::EntitySheet;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::DAMAGE_SIDE_RULE_TEXT;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::MISSILE_DAMAGE_POINTS;
using rtype::game::testing::MISSILE_DAMAGE_TABLE;
using rtype::game::testing::MISSILE_GAMEPLAY;
using rtype::game::testing::RIGHT_HEADING_TABLE;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::ZERO_DAMAGE_TABLE;
using ::testing::HasSubstr;

/**
 * Given the sheet of a missile of a player that takes 3 health points
 * When its gameplay is read
 * Then its damage is 3
 */
TEST_F(EntitySheetReaderFixture, ReadsTheDamageOfAMissile) {
  const EntitySheet sheet =
      read({MISSILE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, RIGHT_HEADING_TABLE,
            MISSILE_DAMAGE_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.damage.value_or(0), MISSILE_DAMAGE_POINTS);
}

/**
 * Given the sheet of a missile without a damage table
 * When its gameplay is read
 * Then it names no damage: it takes MISSILE_DAMAGE
 */
TEST_F(EntitySheetReaderFixture, AMissileWithoutDamageNamesNone) {
  const EntitySheet sheet = read({MISSILE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  RIGHT_HEADING_TABLE, APPEARANCE});

  EXPECT_FALSE(sheet.damage.has_value());
}

/**
 * Given the sheet of a Bydo that names a damage
 * When its gameplay is read
 * Then it is refused: only a missile deals damage
 */
TEST_F(EntitySheetReaderFixture, RefusesADamageOnABydo) {
  EXPECT_THAT(refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                         LEFT_HEADING_TABLE, MISSILE_DAMAGE_TABLE, APPEARANCE}),
              HasSubstr(DAMAGE_SIDE_RULE_TEXT));
}

/**
 * Given the sheet of a missile that takes 0 health points
 * When its gameplay is read
 * Then it is refused for the range of the key
 */
TEST_F(EntitySheetReaderFixture, RefusesADamageOfZero) {
  EXPECT_THROW(static_cast<void>(
                   read({MISSILE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                         RIGHT_HEADING_TABLE, ZERO_DAMAGE_TABLE, APPEARANCE})),
               DataFileValueOutOfRangeException);
}
