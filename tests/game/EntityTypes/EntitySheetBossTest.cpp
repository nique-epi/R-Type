#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetBossTestConstants.hpp"
#include "EntitySheetContactFixtures.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "HurtBeam.hpp"
#include "HurtZone.hpp"
#include "VolleySheet.hpp"

using rtype::game::ActionKey;
using rtype::game::EnemyPhase;
using rtype::game::EntitySheet;
using rtype::game::HurtBeam;
using rtype::game::HurtZone;
using rtype::game::VolleySheet;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::ENEMY_MISSILE_GAMEPLAY;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::GAP_COUNT_DOTTED_KEY;
using rtype::game::testing::HITBOX_DOTTED_KEY;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::MOVED_HITBOX;
using rtype::game::testing::MOVED_HITBOX_OFFSET_X;
using rtype::game::testing::MOVED_HITBOX_OFFSET_Y;
using rtype::game::testing::MOVED_HITBOX_SIDE;
using rtype::game::testing::MOVED_SHIP_HITBOX;
using rtype::game::testing::SHIP_GAMEPLAY;
using rtype::game::testing::SHIP_HITBOX_RULE_TEXT;
using rtype::game::testing::SHIP_MOTION;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::SWEEP_RULE_TEXT;
using rtype::game::testing::SWEEPING_BEAM_ATTACK;
using rtype::game::testing::SWEEPING_BEAM_FROM_DEGREES;
using rtype::game::testing::SWEEPING_BEAM_LENGTH;
using rtype::game::testing::SWEEPING_BEAM_OFFSET_X;
using rtype::game::testing::SWEEPING_BEAM_OFFSET_Y;
using rtype::game::testing::SWEEPING_BEAM_THICKNESS;
using rtype::game::testing::SWEEPING_BEAM_TO_DEGREES;
using rtype::game::testing::SWEEPING_BEAM_UNTIL;
using rtype::game::testing::TO_DEGREES_DOTTED_KEY;
using rtype::game::testing::VOLLEY_ATTACK;
using rtype::game::testing::VOLLEY_BODY;
using rtype::game::testing::VOLLEY_COUNT;
using rtype::game::testing::VOLLEY_DISTANCE;
using rtype::game::testing::VOLLEY_DOTTED_KEY;
using rtype::game::testing::VOLLEY_GAP_COUNT;
using rtype::game::testing::VOLLEY_GAP_FROM;
using rtype::game::testing::VOLLEY_GAP_RULE_TEXT;
using rtype::game::testing::VOLLEY_OF_EVERY_KEY;
using rtype::game::testing::VOLLEY_OF_REQUIRED_KEYS;
using rtype::game::testing::VOLLEY_OFFSET_X;
using rtype::game::testing::VOLLEY_OFFSET_Y;
using rtype::game::testing::VOLLEY_SIDE_RULE_TEXT;
using rtype::game::testing::VOLLEY_SPREAD_DEGREES;
using rtype::game::testing::VOLLEY_STEP_Y;
using rtype::game::testing::VOLLEY_WITH_NO_BODY_LEFT;
using rtype::game::testing::ZONE_WITH_A_SWEEPING_BEAM;
using rtype::game::testing::ZONE_WITH_AN_ENDLESS_SWEEP;
using rtype::game::testing::zoneOf;
using ::testing::HasSubstr;

/**
 * Given the sheet of an enemy whose hitbox is 72 by 72, 76 units left of its
 * Position and 52 above it
 * When its gameplay is read
 * Then its box keeps that size and that offset
 */
TEST_F(EntitySheetReaderFixture, ReadsAMovedHitbox) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, MOVED_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_FLOAT_EQ(sheet.hitbox.width, MOVED_HITBOX_SIDE);
  EXPECT_FLOAT_EQ(sheet.hitbox.height, MOVED_HITBOX_SIDE);
  EXPECT_FLOAT_EQ(sheet.hitbox.offsetX, MOVED_HITBOX_OFFSET_X);
  EXPECT_FLOAT_EQ(sheet.hitbox.offsetY, MOVED_HITBOX_OFFSET_Y);
}

/**
 * Given the sheet of an enemy whose hitbox gives no offset
 * When its gameplay is read
 * Then its box is centered on its Position
 */
TEST_F(EntitySheetReaderFixture, ReadsAHitboxWithoutOffsetAsCentered) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_FLOAT_EQ(sheet.hitbox.offsetX, 0.0F);
  EXPECT_FLOAT_EQ(sheet.hitbox.offsetY, 0.0F);
}

/**
 * Given the sheet of a ship whose box is the right size but moved
 * When its gameplay is read
 * Then it is refused at its hitbox: shipAfter() keeps a centered box on
 * screen
 */
TEST_F(EntitySheetReaderFixture, RefusesAShipWithAMovedHitbox) {
  const std::string message =
      refusalOf({SHIP_GAMEPLAY, MOVED_SHIP_HITBOX, SHIP_MOTION, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(HITBOX_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(SHIP_HITBOX_RULE_TEXT));
}

/**
 * Given the sheet of an enemy whose hurt zone holds a box and a sweeping beam
 * When its gameplay is read
 * Then the zone holds that beam, its pivot, size, angles, action and end
 */
TEST_F(EntitySheetReaderFixture, ReadsABeam) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            ZONE_WITH_A_SWEEPING_BEAM, APPEARANCE});

  const HurtZone& zone = zoneOf(sheet);
  ASSERT_EQ(zone.beams.size(), 1U);
  const HurtBeam& beam = zone.beams.front();
  EXPECT_FLOAT_EQ(beam.offsetX, SWEEPING_BEAM_OFFSET_X);
  EXPECT_FLOAT_EQ(beam.offsetY, SWEEPING_BEAM_OFFSET_Y);
  EXPECT_FLOAT_EQ(beam.length, SWEEPING_BEAM_LENGTH);
  EXPECT_FLOAT_EQ(beam.thickness, SWEEPING_BEAM_THICKNESS);
  EXPECT_FLOAT_EQ(beam.fromDegrees, SWEEPING_BEAM_FROM_DEGREES);
  EXPECT_FLOAT_EQ(beam.toDegrees, SWEEPING_BEAM_TO_DEGREES);
  EXPECT_EQ(beam.activeIn, (ActionKey{.attack = SWEEPING_BEAM_ATTACK,
                                      .phase = EnemyPhase::Strike}));
  EXPECT_EQ(beam.window.until, SWEEPING_BEAM_UNTIL);
}

/**
 * Given the sheet of an enemy whose beam sweeps without naming until_ms
 * When its gameplay is read
 * Then it is refused at to_deg: a sweep needs an end to spread over
 */
TEST_F(EntitySheetReaderFixture, RefusesASweepWithoutAnEnd) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 ZONE_WITH_AN_ENDLESS_SWEEP, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(TO_DEGREES_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(SWEEP_RULE_TEXT));
}

/**
 * Given the sheet of an enemy with a volley that gives every key
 * When its gameplay is read
 * Then the sheet holds that volley, its body still a name
 */
TEST_F(EntitySheetReaderFixture, ReadsAVolley) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            VOLLEY_OF_EVERY_KEY, APPEARANCE});

  ASSERT_EQ(sheet.volleys.size(), 1U);
  const VolleySheet& volley = sheet.volleys.front();
  EXPECT_EQ(volley.on,
            (ActionKey{.attack = VOLLEY_ATTACK, .phase = EnemyPhase::Strike}));
  EXPECT_EQ(volley.body, VOLLEY_BODY);
  EXPECT_EQ(volley.count, VOLLEY_COUNT);
  EXPECT_FLOAT_EQ(volley.offsetX, VOLLEY_OFFSET_X);
  EXPECT_FLOAT_EQ(volley.offsetY, VOLLEY_OFFSET_Y);
  EXPECT_FLOAT_EQ(volley.stepY, VOLLEY_STEP_Y);
  EXPECT_FLOAT_EQ(volley.spreadDegrees, VOLLEY_SPREAD_DEGREES);
  EXPECT_FLOAT_EQ(volley.distance, VOLLEY_DISTANCE);
  EXPECT_EQ(volley.gapFrom, VOLLEY_GAP_FROM);
  EXPECT_EQ(volley.gapCount, VOLLEY_GAP_COUNT);
}

/**
 * Given the sheet of an enemy with a volley of its required keys only
 * When its gameplay is read
 * Then its step, spread, distance and gap are 0
 */
TEST_F(EntitySheetReaderFixture, ReadsTheOptionalKeysOfAVolleyAsZero) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            VOLLEY_OF_REQUIRED_KEYS, APPEARANCE});

  ASSERT_EQ(sheet.volleys.size(), 1U);
  const VolleySheet& volley = sheet.volleys.front();
  EXPECT_FLOAT_EQ(volley.stepX, 0.0F);
  EXPECT_FLOAT_EQ(volley.stepY, 0.0F);
  EXPECT_FLOAT_EQ(volley.spreadDegrees, 0.0F);
  EXPECT_FLOAT_EQ(volley.distance, 0.0F);
  EXPECT_EQ(volley.gapFrom, 0U);
  EXPECT_EQ(volley.gapCount, 0U);
}

/**
 * Given the sheet of an enemy with a volley of two bodies whose gap takes
 * both
 * When its gameplay is read
 * Then it is refused at gap_count: a volley fires at least one body
 */
TEST_F(EntitySheetReaderFixture, RefusesAGapThatLeavesNoBody) {
  const std::string message =
      refusalOf({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
                 VOLLEY_WITH_NO_BODY_LEFT, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(GAP_COUNT_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(VOLLEY_GAP_RULE_TEXT));
}

/**
 * Given the sheet of a missile of an enemy with a volley
 * When its gameplay is read
 * Then it is refused at the volley: only an enemy fires volleys
 */
TEST_F(EntitySheetReaderFixture, RefusesAVolleyOfAMissile) {
  const std::string message =
      refusalOf({ENEMY_MISSILE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                 LEFT_HEADING_TABLE, VOLLEY_OF_REQUIRED_KEYS, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(VOLLEY_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(VOLLEY_SIDE_RULE_TEXT));
}
