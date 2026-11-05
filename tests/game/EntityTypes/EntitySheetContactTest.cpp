#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetContactFixtures.hpp"
#include "EntitySheetContactTestConstants.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"
#include "HurtZone.hpp"

using rtype::engine::DataFileTablesExpectedException;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::UnknownDataFileChoiceException;
using rtype::game::ActionKey;
using rtype::game::EnemyPhase;
using rtype::game::EntitySheet;
using rtype::game::HurtBox;
using rtype::game::HurtWindow;
using rtype::game::testing::ACTION_TOGETHER_RULE_TEXT;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::ATTACK_DOTTED_KEY;
using rtype::game::testing::ATTACK_OF_EVERY_PHASE;
using rtype::game::testing::BOX_ENDING_AS_IT_STARTS;
using rtype::game::testing::BOX_OF_AN_UNKNOWN_PHASE;
using rtype::game::testing::BOX_WITH_A_PHASE_ONLY;
using rtype::game::testing::BOX_WITH_AN_ATTACK_ONLY;
using rtype::game::testing::BOX_WITH_AN_UNKNOWN_KEY;
using rtype::game::testing::BOX_WITHOUT_HEIGHT;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::EMPTY_ZONE_RULE_TEXT;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::ENEMY_MISSILE_GAMEPLAY;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::EVERY_PHASE_OF_ATTACK_TWO;
using rtype::game::testing::FIRST_BOX_HEIGHT;
using rtype::game::testing::FIRST_BOX_OFFSET_X;
using rtype::game::testing::FIRST_BOX_OFFSET_Y;
using rtype::game::testing::FIRST_BOX_WIDTH;
using rtype::game::testing::FRAGILE_DOTTED_KEY;
using rtype::game::testing::FRAGILE_SIDE_RULE_TEXT;
using rtype::game::testing::FRAGILE_WHILE_STUNNED;
using rtype::game::testing::FRAGILE_WITHOUT_PHASE;
using rtype::game::testing::HURT_BOXES_IN_EVERY_PHASE;
using rtype::game::testing::HURT_BOXES_IN_TWO;
using rtype::game::testing::HURT_ZONE_DOTTED_KEY;
using rtype::game::testing::HURT_ZONE_SIDE_RULE_TEXT;
using rtype::game::testing::HURT_ZONE_WITH_A_LONE_TABLE;
using rtype::game::testing::HURT_ZONE_WITH_AN_EMPTY_ARRAY;
using rtype::game::testing::HURT_ZONE_WITHOUT_BOX;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::OPEN_ENDED_BOX;
using rtype::game::testing::PHASE_DOTTED_KEY;
using rtype::game::testing::SECOND_BOX_ATTACK;
using rtype::game::testing::SECOND_BOX_OFFSET_Y;
using rtype::game::testing::SECOND_BOX_TOO_WIDE;
using rtype::game::testing::SECOND_WIDTH_DOTTED_KEY;
using rtype::game::testing::SHIP_GAMEPLAY;
using rtype::game::testing::SHIP_HITBOX;
using rtype::game::testing::SHIP_MOTION;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::SURVIVES_CONTACT_MARKER;
using rtype::game::testing::SURVIVES_CONTACT_SIDE_RULE_TEXT;
using rtype::game::testing::SURVIVES_CONTACT_WITH_A_KEY;
using rtype::game::testing::TIMED_BOX;
using rtype::game::testing::TIMED_BOX_FROM;
using rtype::game::testing::TIMED_BOX_UNTIL;
using rtype::game::testing::TIMED_BOX_WITHOUT_PHASE;
using rtype::game::testing::TIMING_RULE_TEXT;
using rtype::game::testing::TWO_HURT_BOXES;
using rtype::game::testing::UNTIL_DOTTED_KEY;
using rtype::game::testing::WINDOW_RULE_TEXT;
using rtype::game::testing::zoneOf;
using ::testing::HasSubstr;

/**
 * Given the sheet of an enemy with two boxes in its hurt zone
 * When its gameplay is read
 * Then the zone holds two boxes in the order of the file, with their offset
 * and their size in logical units
 */
TEST_F(EntitySheetReaderFixture, ReadsTheBoxesOfAHurtZoneInTheOrderOfTheFile) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            TWO_HURT_BOXES, APPEARANCE});

  ASSERT_TRUE(sheet.hurtZone.has_value());
  ASSERT_EQ(zoneOf(sheet).boxes.size(), HURT_BOXES_IN_TWO);
  const HurtBox& first = zoneOf(sheet).boxes.front();
  EXPECT_FLOAT_EQ(first.offsetX, FIRST_BOX_OFFSET_X);
  EXPECT_FLOAT_EQ(first.offsetY, FIRST_BOX_OFFSET_Y);
  EXPECT_FLOAT_EQ(first.width, FIRST_BOX_WIDTH);
  EXPECT_FLOAT_EQ(first.height, FIRST_BOX_HEIGHT);
  EXPECT_FLOAT_EQ(zoneOf(sheet).boxes.back().offsetY, SECOND_BOX_OFFSET_Y);
}

/**
 * Given a box that names neither an attack nor a phase
 * When the gameplay is read
 * Then the box hurts all the time
 */
TEST_F(EntitySheetReaderFixture, ABoxThatNamesNoActionHurtsAllTheTime) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            TWO_HURT_BOXES, APPEARANCE});

  ASSERT_TRUE(sheet.hurtZone.has_value());
  EXPECT_FALSE(zoneOf(sheet).boxes.front().activeIn.has_value());
}

/**
 * Given a box of attack 1 and of the phase strike
 * When the gameplay is read
 * Then the box hurts in the strike of attack 1
 */
TEST_F(EntitySheetReaderFixture, ABoxHurtsInTheAttackAndThePhaseItNames) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            TWO_HURT_BOXES, APPEARANCE});

  ASSERT_TRUE(sheet.hurtZone.has_value());
  EXPECT_EQ(
      zoneOf(sheet).boxes.back().activeIn,
      (ActionKey{.attack = SECOND_BOX_ATTACK, .phase = EnemyPhase::Strike}));
}

/**
 * Given four boxes of attack 2, one for each phase name
 * When the gameplay is read
 * Then they hurt in Signal, Strike, Recover and Enraged, in that order
 */
TEST_F(EntitySheetReaderFixture, ReadsEachPhaseByItsName) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            EVERY_PHASE_OF_ATTACK_TWO, APPEARANCE});

  ASSERT_TRUE(sheet.hurtZone.has_value());
  ASSERT_EQ(zoneOf(sheet).boxes.size(), HURT_BOXES_IN_EVERY_PHASE);
  EXPECT_EQ(zoneOf(sheet).boxes.at(0).activeIn,
            (ActionKey{.attack = ATTACK_OF_EVERY_PHASE,
                       .phase = EnemyPhase::Signal}));
  EXPECT_EQ(zoneOf(sheet).boxes.at(1).activeIn,
            (ActionKey{.attack = ATTACK_OF_EVERY_PHASE,
                       .phase = EnemyPhase::Strike}));
  EXPECT_EQ(zoneOf(sheet).boxes.at(2).activeIn,
            (ActionKey{.attack = ATTACK_OF_EVERY_PHASE,
                       .phase = EnemyPhase::Recover}));
  EXPECT_EQ(zoneOf(sheet).boxes.at(3).activeIn,
            (ActionKey{.attack = ATTACK_OF_EVERY_PHASE,
                       .phase = EnemyPhase::Enraged}));
}

/**
 * Given the sheet of an enemy with neither a hurt zone nor the marker
 * When its gameplay is read
 * Then it has no hurt zone and a contact destroys it
 */
TEST_F(EntitySheetReaderFixture, ContactPartsLeftOutGiveNothing) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_FALSE(sheet.hurtZone.has_value());
  EXPECT_FALSE(sheet.survivesContact);
}

/**
 * Given the sheet of an enemy with the table survives_contact
 * When its gameplay is read
 * Then the enemy survives a contact
 */
TEST_F(EntitySheetReaderFixture, ReadsTheMarkerOfAnEnemyThatSurvivesContact) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            SURVIVES_CONTACT_MARKER, APPEARANCE});

  EXPECT_TRUE(sheet.survivesContact);
}

/**
 * Given a hurt zone with no box key
 * When its gameplay is read
 * Then the file is refused for the missing key
 */
TEST_F(EntitySheetReaderFixture, RefusesAHurtZoneWithoutTheKeyBox) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 HURT_ZONE_WITHOUT_BOX, APPEARANCE});

  EXPECT_THAT(message, HasSubstr("gameplay.hurt_zone.box is missing"));
}

/**
 * Given a hurt zone whose array of boxes is empty
 * When its gameplay is read
 * Then the file is refused: the refusal names the zone and the rule
 */
TEST_F(EntitySheetReaderFixture, RefusesAHurtZoneWithNoBox) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 HURT_ZONE_WITH_AN_EMPTY_ARRAY, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(HURT_ZONE_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(EMPTY_ZONE_RULE_TEXT));
}

/**
 * Given a hurt zone whose box is one table and not an array of tables
 * When its gameplay is read
 * Then a DataFileTablesExpectedException is thrown
 */
TEST_F(EntitySheetReaderFixture, RefusesABoxThatIsNotInAnArrayOfTables) {
  EXPECT_THROW(
      static_cast<void>(
          read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                HURT_ZONE_WITH_A_LONE_TABLE, APPEARANCE})),
      DataFileTablesExpectedException);
}

/**
 * Given a box that names an attack and no phase
 * When its gameplay is read
 * Then the refusal names the attack key and the rule
 */
TEST_F(EntitySheetReaderFixture, RefusesABoxThatNamesAnAttackWithoutAPhase) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 BOX_WITH_AN_ATTACK_ONLY, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(ATTACK_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(ACTION_TOGETHER_RULE_TEXT));
}

/**
 * Given a box that names a phase and no attack
 * When its gameplay is read
 * Then the refusal names the phase key and the rule
 */
TEST_F(EntitySheetReaderFixture, RefusesABoxThatNamesAPhaseWithoutAnAttack) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 BOX_WITH_A_PHASE_ONLY, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(PHASE_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(ACTION_TOGETHER_RULE_TEXT));
}

/**
 * Given a box whose phase is misspelt
 * When its gameplay is read
 * Then an UnknownDataFileChoiceException is thrown
 */
TEST_F(EntitySheetReaderFixture, RefusesAPhaseNoPhaseIsCalled) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX,
                                       STEADY_MOTION, LEFT_HEADING_TABLE,
                                       BOX_OF_AN_UNKNOWN_PHASE, APPEARANCE})),
               UnknownDataFileChoiceException);
}

/**
 * Given a box with a key a box does not have
 * When its gameplay is read
 * Then the refusal names that key
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownKeyInABox) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 BOX_WITH_AN_UNKNOWN_KEY, APPEARANCE});

  EXPECT_THAT(message, HasSubstr("gameplay.hurt_zone.box[0].damage"));
}

/**
 * Given a box without its height
 * When its gameplay is read
 * Then the refusal says the height is missing
 */
TEST_F(EntitySheetReaderFixture, RefusesABoxWithoutHeight) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 BOX_WITHOUT_HEIGHT, APPEARANCE});

  EXPECT_THAT(message,
              HasSubstr("gameplay.hurt_zone.box[0].height is missing"));
}

/**
 * Given a second box 1025 units wide, one more than a box accepts
 * When its gameplay is read
 * Then the refusal names the width of the second box
 */
TEST_F(EntitySheetReaderFixture, RefusesABoxTooWideAndNamesItsIndex) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 SECOND_BOX_TOO_WIDE, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(SECOND_WIDTH_DOTTED_KEY));
}

/**
 * Given the sheet of a ship with a hurt zone
 * When its gameplay is read
 * Then the refusal names the zone and says only an enemy has one
 */
TEST_F(EntitySheetReaderFixture, RefusesAHurtZoneOnAShip) {
  const std::string message = refusalOf(
      {SHIP_GAMEPLAY, SHIP_HITBOX, SHIP_MOTION, TWO_HURT_BOXES, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(HURT_ZONE_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(HURT_ZONE_SIDE_RULE_TEXT));
}

/**
 * Given the sheet of a missile of an enemy with the marker survives_contact
 * When its gameplay is read
 * Then the refusal says only an enemy survives a contact
 */
TEST_F(EntitySheetReaderFixture, RefusesTheMarkerOnAMissile) {
  const std::string message =
      refusalOf({ENEMY_MISSILE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                 LEFT_HEADING_TABLE, SURVIVES_CONTACT_MARKER, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(SURVIVES_CONTACT_SIDE_RULE_TEXT));
}

/**
 * Given the marker survives_contact that holds a key
 * When its gameplay is read
 * Then the refusal names that key
 */
TEST_F(EntitySheetReaderFixture, RefusesAKeyInTheMarker) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 SURVIVES_CONTACT_WITH_A_KEY, APPEARANCE});

  EXPECT_THAT(message, HasSubstr("gameplay.survives_contact.survives"));
}

/**
 * Given a box of the strike of attack 1 with from_ms 70 and until_ms 140
 * When the gameplay is read
 * Then the box hurts from 70 ms into the strike until 140 ms
 */
TEST_F(EntitySheetReaderFixture, ReadsTheWindowOfATimedBox) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, TIMED_BOX, APPEARANCE});

  ASSERT_TRUE(sheet.hurtZone.has_value());
  EXPECT_EQ(zoneOf(sheet).boxes.front().window,
            (HurtWindow{.from = TIMED_BOX_FROM, .until = TIMED_BOX_UNTIL}));
}

/**
 * Given a box with from_ms and no until_ms
 * When the gameplay is read
 * Then the box hurts from that time to the end of its phase
 */
TEST_F(EntitySheetReaderFixture, ABoxWithoutUntilHurtsToTheEndOfItsPhase) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            OPEN_ENDED_BOX, APPEARANCE});

  ASSERT_TRUE(sheet.hurtZone.has_value());
  EXPECT_EQ(zoneOf(sheet).boxes.front().window,
            (HurtWindow{.from = TIMED_BOX_FROM, .until = std::nullopt}));
}

/**
 * Given a box that hurts until a time and names no attack nor phase
 * When its gameplay is read
 * Then the refusal names until_ms and the rule
 */
TEST_F(EntitySheetReaderFixture, RefusesATimedBoxThatNamesNoPhase) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 TIMED_BOX_WITHOUT_PHASE, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(UNTIL_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(TIMING_RULE_TEXT));
}

/**
 * Given a box that stops hurting at 70 ms, the time it starts
 * When its gameplay is read
 * Then the refusal names until_ms and the rule
 */
TEST_F(EntitySheetReaderFixture, RefusesABoxThatEndsAsItStarts) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 BOX_ENDING_AS_IT_STARTS, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(UNTIL_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(WINDOW_RULE_TEXT));
}

/**
 * Given the sheet of an enemy fragile in the recover of its attack 0
 * When its gameplay is read
 * Then it is fragile in that recover
 */
TEST_F(EntitySheetReaderFixture, ReadsTheActionAnEnemyIsFragileIn) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            FRAGILE_WHILE_STUNNED, APPEARANCE});

  EXPECT_EQ(sheet.fragileIn,
            (ActionKey{.attack = 0, .phase = EnemyPhase::Recover}));
}

/**
 * Given a fragile table that names an attack and no phase
 * When the gameplay is read
 * Then a MissingDataFileKeyException is thrown
 */
TEST_F(EntitySheetReaderFixture, RefusesAFragileTableWithoutPhase) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX,
                                       STEADY_MOTION, LEFT_HEADING_TABLE,
                                       FRAGILE_WITHOUT_PHASE, APPEARANCE})),
               MissingDataFileKeyException);
}

/**
 * Given the sheet of a missile that declares itself fragile
 * When its gameplay is read
 * Then the refusal names the fragile table and the rule
 */
TEST_F(EntitySheetReaderFixture, RefusesAFragileMissile) {
  const std::string message =
      refusalOf({ENEMY_MISSILE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                 LEFT_HEADING_TABLE, FRAGILE_WHILE_STUNNED, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(FRAGILE_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(FRAGILE_SIDE_RULE_TEXT));
}
