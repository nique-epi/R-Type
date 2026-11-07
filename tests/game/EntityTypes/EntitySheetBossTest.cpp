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

using rtype::game::ActionKey;
using rtype::game::EnemyPhase;
using rtype::game::EntitySheet;
using rtype::game::HurtBeam;
using rtype::game::HurtZone;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::EntitySheetReaderFixture;
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
