#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "BehaviourConstants.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetFixtures.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "EntityType.hpp"
#include "EntityTypesTestConstants.hpp"
#include "GameException.hpp"
#include "MotionKind.hpp"

using rtype::engine::DataFileSyntaxException;
using rtype::engine::DataFileTableExpectedException;
using rtype::engine::DataFileValueOutOfRangeException;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::UnknownDataFileChoiceException;
using rtype::engine::UnknownDataFileKeyException;
using rtype::game::EntitySheet;
using rtype::game::EntityType;
using rtype::game::InconsistentEntitySheetException;
using rtype::game::MotionKind;
using rtype::game::WEAVE_BEHAVIOUR_NAME;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::APPEARANCE_AS_A_NUMBER;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::EMPTY_BEHAVIOUR_TABLE;
using rtype::game::testing::EMPTY_HITBOX;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::ENEMY_GUN;
using rtype::game::testing::ENEMY_MISSILE_NAME_IN_TESTS;
using rtype::game::testing::ENEMY_PILOTED_BY_A_PLAYER;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::firstShotTicksOf;
using rtype::game::testing::GAMEPLAY_OF_AN_UNKNOWN_PILOT;
using rtype::game::testing::GAMEPLAY_OF_AN_UNKNOWN_SIDE;
using rtype::game::testing::GAMEPLAY_WITH_AN_UNKNOWN_KEY;
using rtype::game::testing::GAMEPLAY_WITHOUT_PILOT;
using rtype::game::testing::GAMEPLAY_WITHOUT_SIDE;
using rtype::game::testing::GUN_WITHOUT_PROJECTILE;
using rtype::game::testing::HEADING_TABLE_WITH_AN_UNKNOWN_SETTING;
using rtype::game::testing::HEALTH_TABLE;
using rtype::game::testing::HITBOX_WITHOUT_HEIGHT;
using rtype::game::testing::intervalTicksOf;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::MISSILE_GAMEPLAY;
using rtype::game::testing::NARROW_SHIP_HITBOX;
using rtype::game::testing::NO_HEALTH_TABLE;
using rtype::game::testing::PILOT_DOTTED_KEY;
using rtype::game::testing::pilotBehaviourName;
using rtype::game::testing::PLAYER_PILOT_RULE_TEXT;
using rtype::game::testing::PLAYER_PILOTED_BY_A_BEHAVIOUR;
using rtype::game::testing::projectileNameOf;
using rtype::game::testing::READ_SHEET_SOURCE;
using rtype::game::testing::REQUIRED_BYDO_HEALTH;
using rtype::game::testing::REQUIRED_BYDO_SHOT_TICKS;
using rtype::game::testing::REQUIRED_BYDO_SIZE;
using rtype::game::testing::REQUIRED_BYDO_SPEED;
using rtype::game::testing::REQUIRED_DRIFTER_POINTS;
using rtype::game::testing::REQUIRED_SHIP_FIRST_SHOT_TICKS;
using rtype::game::testing::REQUIRED_SHIP_SHOT_TICKS;
using rtype::game::testing::REQUIRED_WEAVE;
using rtype::game::testing::SCORE_TABLE;
using rtype::game::testing::settingsOf;
using rtype::game::testing::SHIP_GAMEPLAY;
using rtype::game::testing::SHIP_GUN;
using rtype::game::testing::SHIP_HITBOX;
using rtype::game::testing::SHIP_MOTION;
using rtype::game::testing::SHIP_MOTION_WITH_A_TOP_SPEED;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::STEADY_MOTION_IN_A_FLOAT;
using rtype::game::testing::STEADY_MOTION_WITHOUT_TOP_SPEED;
using rtype::game::testing::TEXT_THAT_IS_NOT_TOML;
using rtype::game::testing::TOO_FAR_HEADING_TABLE;
using rtype::game::testing::UNKNOWN_MOTION;
using rtype::game::testing::UNKNOWN_ROOT_TABLE;
using rtype::game::testing::WEAVE_TABLE;
using rtype::game::testing::WEAVE_TABLE_PERIOD_FIRST;
using rtype::game::testing::WEAVE_TABLE_WITHOUT_PERIOD;
using rtype::game::testing::WEAVER_GAMEPLAY;
using ::testing::ElementsAreArray;
using ::testing::HasSubstr;

/**
 * Given the sheet of a Drifter, of side enemy
 * When its gameplay is read
 * Then the sheet is of side enemy
 */
TEST_F(EntitySheetReaderFixture, ReadsTheSideOfTheSheet) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.side, EntityType::Enemy);
}

/**
 * Given the sheet of a Drifter, whose hitbox is 96 by 96
 * When its gameplay is read
 * Then its box is 96 by 96 logical units
 */
TEST_F(EntitySheetReaderFixture, ReadsTheHitboxInLogicalUnits) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_FLOAT_EQ(sheet.hitbox.width, REQUIRED_BYDO_SIZE);
  EXPECT_FLOAT_EQ(sheet.hitbox.height, REQUIRED_BYDO_SIZE);
}

/**
 * Given the sheet of a Drifter, which moves steadily at 240
 * When its gameplay is read
 * Then it moves steadily at 240 logical units per second
 */
TEST_F(EntitySheetReaderFixture, ReadsASteadyMotionWithItsTopSpeed) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.motion.kind, MotionKind::Steady);
  EXPECT_FLOAT_EQ(sheet.motion.topSpeed, REQUIRED_BYDO_SPEED);
}

/**
 * Given the sheet of a ship, which moves as a ship
 * When its gameplay is read
 * Then it moves as a ship, with no top speed of its own
 */
TEST_F(EntitySheetReaderFixture, AShipMovesAsAShipWithNoTopSpeed) {
  const EntitySheet sheet =
      read({SHIP_GAMEPLAY, SHIP_HITBOX, SHIP_MOTION, APPEARANCE});

  EXPECT_EQ(sheet.motion.kind, MotionKind::Ship);
  EXPECT_FLOAT_EQ(sheet.motion.topSpeed, 0.0F);
}

/**
 * Given the sheet of a ship, piloted by its player
 * When its gameplay is read
 * Then no behaviour pilots it
 */
TEST_F(EntitySheetReaderFixture, APlayerPilotNamesNoBehaviour) {
  const EntitySheet sheet =
      read({SHIP_GAMEPLAY, SHIP_HITBOX, SHIP_MOTION, APPEARANCE});

  EXPECT_FALSE(sheet.pilot.has_value());
}

/**
 * Given the sheet of a Weaver, piloted by the weave
 * When its gameplay is read
 * Then the weave pilots it
 */
TEST_F(EntitySheetReaderFixture, ReadsTheBehaviourOfThePilot) {
  const EntitySheet sheet = read(
      {WEAVER_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, WEAVE_TABLE, APPEARANCE});

  EXPECT_EQ(pilotBehaviourName(sheet), WEAVE_BEHAVIOUR_NAME);
}

/**
 * Given the sheet of a Weaver, whose behaviour table gives a period of 2000
 * then an amplitude of 48
 * When its gameplay is read
 * Then the pilot holds the amplitude, then the period, the order the weave
 * declares them in
 */
TEST_F(EntitySheetReaderFixture,
       ReadsTheSettingsInTheOrderTheBehaviourDeclares) {
  const EntitySheet sheet = read({WEAVER_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  WEAVE_TABLE_PERIOD_FIRST, APPEARANCE});

  EXPECT_THAT(settingsOf(sheet), ElementsAreArray(REQUIRED_WEAVE));
}

/**
 * Given the sheet of a Drifter of 3 health points
 * When its gameplay is read
 * Then it has 3 health points
 */
TEST_F(EntitySheetReaderFixture, ReadsTheHealth) {
  const EntitySheet sheet =
      read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE,
            HEALTH_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.health.value_or(0), REQUIRED_BYDO_HEALTH);
}

/**
 * Given the sheet of a Drifter worth 100 points
 * When its gameplay is read
 * Then its destroyer earns 100 points
 */
TEST_F(EntitySheetReaderFixture, ReadsThePointsOfItsScore) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, SCORE_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.points.value_or(0U), REQUIRED_DRIFTER_POINTS);
}

/**
 * Given the sheet of a missile, with no table health, score or gun
 * When its gameplay is read
 * Then it has no health, is worth no points and fires nothing
 */
TEST_F(EntitySheetReaderFixture, TablesLeftOutGiveNothing) {
  const EntitySheet sheet = read({MISSILE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_FALSE(sheet.health.has_value());
  EXPECT_FALSE(sheet.points.has_value());
  EXPECT_FALSE(sheet.gun.has_value());
}

/**
 * Given the sheet of a Drifter whose gun fires every 1500 ms
 * When its gameplay is read
 * Then its gun fires every 90 ticks
 */
TEST_F(EntitySheetReaderFixture, TurnsAnIntervalOf1500MsInto90Ticks) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, ENEMY_GUN, APPEARANCE});

  EXPECT_EQ(intervalTicksOf(sheet), REQUIRED_BYDO_SHOT_TICKS);
}

/**
 * Given the sheet of a ship whose gun fires every 333 ms, 19.98 ticks
 * When its gameplay is read
 * Then its gun fires every 20 ticks, the nearest whole number, 3 shots per
 * second as today
 */
TEST_F(EntitySheetReaderFixture, TurnsAnIntervalOf333MsIntoTheNearest20Ticks) {
  const EntitySheet sheet =
      read({SHIP_GAMEPLAY, SHIP_HITBOX, SHIP_MOTION, SHIP_GUN, APPEARANCE});

  EXPECT_EQ(intervalTicksOf(sheet), REQUIRED_SHIP_SHOT_TICKS);
}

/**
 * Given the sheet of a ship whose first shot comes after 0 ms
 * When its gameplay is read
 * Then its gun is ready at once
 */
TEST_F(EntitySheetReaderFixture, AFirstShotAfter0MsIsReadyAtOnce) {
  const EntitySheet sheet =
      read({SHIP_GAMEPLAY, SHIP_HITBOX, SHIP_MOTION, SHIP_GUN, APPEARANCE});

  EXPECT_EQ(firstShotTicksOf(sheet), REQUIRED_SHIP_FIRST_SHOT_TICKS);
}

/**
 * Given the sheet of a Drifter whose first shot comes after 1500 ms
 * When its gameplay is read
 * Then its gun waits 90 ticks before its first shot
 */
TEST_F(EntitySheetReaderFixture, AFirstShotAfter1500MsWaits90Ticks) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, ENEMY_GUN, APPEARANCE});

  EXPECT_EQ(firstShotTicksOf(sheet), REQUIRED_BYDO_SHOT_TICKS);
}

/**
 * Given the sheet of a Drifter whose gun fires enemy_missile
 * When its gameplay is read
 * Then the gun keeps that name, for the catalog to find the sheet
 */
TEST_F(EntitySheetReaderFixture, KeepsTheNameOfTheProjectile) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, ENEMY_GUN, APPEARANCE});

  EXPECT_EQ(projectileNameOf(sheet), ENEMY_MISSILE_NAME_IN_TESTS);
}

/**
 * Given a sheet of side enemy piloted by a player
 * When its gameplay is read
 * Then the refusal names the file, the dotted key of the pilot and the rule
 */
TEST_F(EntitySheetReaderFixture, ARefusedRuleNamesTheFileTheKeyAndTheRule) {
  const std::string message = refusalOf(
      {ENEMY_PILOTED_BY_A_PLAYER, BYDO_HITBOX, STEADY_MOTION, APPEARANCE});

  EXPECT_THAT(message, HasSubstr(READ_SHEET_SOURCE));
  EXPECT_THAT(message, HasSubstr(PILOT_DOTTED_KEY));
  EXPECT_THAT(message, HasSubstr(PLAYER_PILOT_RULE_TEXT));
}

/**
 * Given a text that is not TOML
 * When its gameplay is read
 * Then it is refused before any key is read
 */
TEST_F(EntitySheetReaderFixture, RefusesATextThatIsNotToml) {
  EXPECT_THROW(static_cast<void>(read({TEXT_THAT_IS_NOT_TOML})),
               DataFileSyntaxException);
}

/**
 * Given a sheet with an appearance and no table gameplay
 * When its gameplay is read
 * Then it is refused: the server needs the gameplay of every type
 */
TEST_F(EntitySheetReaderFixture, RefusesASheetWithoutGameplay) {
  EXPECT_THROW(static_cast<void>(read({APPEARANCE})),
               MissingDataFileKeyException);
}

/**
 * Given a sheet of a Drifter without the table appearance
 * When its gameplay is read
 * Then it is refused: the client needs the look of every type
 */
TEST_F(EntitySheetReaderFixture, RefusesASheetWithoutAppearance) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX,
                                       STEADY_MOTION, LEFT_HEADING_TABLE})),
               MissingDataFileKeyException);
}

/**
 * Given a sheet whose appearance is a number
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesAnAppearanceThatIsNotATable) {
  EXPECT_THROW(
      static_cast<void>(read({APPEARANCE_AS_A_NUMBER, ENEMY_GAMEPLAY,
                              BYDO_HITBOX, STEADY_MOTION, LEFT_HEADING_TABLE})),
      DataFileTableExpectedException);
}

/**
 * Given a sheet of a Drifter with a table sound at its root
 * When its gameplay is read
 * Then it is refused, so a misspelt table never goes unnoticed
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownTableAtTheRoot) {
  EXPECT_THROW(static_cast<void>(
                   read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                         LEFT_HEADING_TABLE, APPEARANCE, UNKNOWN_ROOT_TABLE})),
               UnknownDataFileKeyException);
}

/**
 * Given a sheet whose gameplay holds a key lives
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownKeyInGameplay) {
  EXPECT_THROW(
      static_cast<void>(read({GAMEPLAY_WITH_AN_UNKNOWN_KEY, BYDO_HITBOX,
                              STEADY_MOTION, LEFT_HEADING_TABLE, APPEARANCE})),
      UnknownDataFileKeyException);
}

/**
 * Given a sheet whose gameplay has no side
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesASheetWithoutSide) {
  EXPECT_THROW(
      static_cast<void>(read({GAMEPLAY_WITHOUT_SIDE, BYDO_HITBOX, STEADY_MOTION,
                              LEFT_HEADING_TABLE, APPEARANCE})),
      MissingDataFileKeyException);
}

/**
 * Given a sheet whose side is enmy
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownSide) {
  EXPECT_THROW(
      static_cast<void>(read({GAMEPLAY_OF_AN_UNKNOWN_SIDE, BYDO_HITBOX,
                              STEADY_MOTION, LEFT_HEADING_TABLE, APPEARANCE})),
      UnknownDataFileChoiceException);
}

/**
 * Given a sheet whose gameplay has no pilot
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesASheetWithoutPilot) {
  EXPECT_THROW(static_cast<void>(read({GAMEPLAY_WITHOUT_PILOT, BYDO_HITBOX,
                                       STEADY_MOTION, APPEARANCE})),
               MissingDataFileKeyException);
}

/**
 * Given a sheet piloted by zigzag, which no behaviour is named
 * When its gameplay is read
 * Then it is refused before the server listens
 */
TEST_F(EntitySheetReaderFixture, RefusesAPilotThatNamesNoBehaviour) {
  EXPECT_THROW(
      static_cast<void>(read({GAMEPLAY_OF_AN_UNKNOWN_PILOT, BYDO_HITBOX,
                              STEADY_MOTION, APPEARANCE})),
      UnknownDataFileChoiceException);
}

/**
 * Given a sheet of a Drifter without its hitbox
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesASheetWithoutHitbox) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_GAMEPLAY, STEADY_MOTION,
                                       LEFT_HEADING_TABLE, APPEARANCE})),
               MissingDataFileKeyException);
}

/**
 * Given a sheet whose hitbox is 0 units wide
 * When its gameplay is read
 * Then it is refused: a width is 1 to 1024
 */
TEST_F(EntitySheetReaderFixture, RefusesAnEmptyHitbox) {
  EXPECT_THROW(
      static_cast<void>(read({ENEMY_GAMEPLAY, EMPTY_HITBOX, STEADY_MOTION,
                              LEFT_HEADING_TABLE, APPEARANCE})),
      DataFileValueOutOfRangeException);
}

/**
 * Given a sheet whose hitbox has a width only
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesAHitboxWithoutHeight) {
  EXPECT_THROW(
      static_cast<void>(read({ENEMY_GAMEPLAY, HITBOX_WITHOUT_HEIGHT,
                              STEADY_MOTION, LEFT_HEADING_TABLE, APPEARANCE})),
      MissingDataFileKeyException);
}

/**
 * Given a sheet of a Drifter without its motion
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesASheetWithoutMotion) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX,
                                       LEFT_HEADING_TABLE, APPEARANCE})),
               MissingDataFileKeyException);
}

/**
 * Given a sheet whose motion is of kind drift
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesAnUnknownMotion) {
  EXPECT_THROW(
      static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX, UNKNOWN_MOTION,
                              LEFT_HEADING_TABLE, APPEARANCE})),
      UnknownDataFileChoiceException);
}

/**
 * Given a sheet whose steady motion has no top speed
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesASteadyMotionWithoutTopSpeed) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX,
                                       STEADY_MOTION_WITHOUT_TOP_SPEED,
                                       LEFT_HEADING_TABLE, APPEARANCE})),
               MissingDataFileKeyException);
}

/**
 * Given a sheet whose top speed is 240.0
 * When its gameplay is read
 * Then it is refused: every number of a sheet is an integer
 */
TEST_F(EntitySheetReaderFixture, RefusesATopSpeedWrittenAsAFloat) {
  EXPECT_THROW(static_cast<void>(
                   read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION_IN_A_FLOAT,
                         LEFT_HEADING_TABLE, APPEARANCE})),
               DataFileValueOutOfRangeException);
}

/**
 * Given a sheet of a ship whose motion gives a top speed
 * When its gameplay is read
 * Then it is refused: a ship moves at the speed of shipAfter()
 */
TEST_F(EntitySheetReaderFixture, RefusesATopSpeedOnAShip) {
  EXPECT_THROW(
      static_cast<void>(read({SHIP_GAMEPLAY, SHIP_HITBOX,
                              SHIP_MOTION_WITH_A_TOP_SPEED, APPEARANCE})),
      UnknownDataFileKeyException);
}

/**
 * Given a sheet piloted by the weave, without a behaviour table
 * When its gameplay is read
 * Then it is refused: the weave declares two settings
 */
TEST_F(EntitySheetReaderFixture, RefusesAWeaveWithoutItsBehaviourTable) {
  EXPECT_THROW(static_cast<void>(read(
                   {WEAVER_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION, APPEARANCE})),
               MissingDataFileKeyException);
}

/**
 * Given a weave whose behaviour table has no period
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture,
       RefusesABehaviourTableWithoutOneOfItsSettings) {
  EXPECT_THROW(
      static_cast<void>(read({WEAVER_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                              WEAVE_TABLE_WITHOUT_PERIOD, APPEARANCE})),
      MissingDataFileKeyException);
}

/**
 * Given a straight body whose behaviour table also gives a speed
 * When its gameplay is read
 * Then it is refused, so a misspelt setting never runs with a default
 */
TEST_F(EntitySheetReaderFixture, RefusesASettingTheBehaviourDoesNotDeclare) {
  EXPECT_THROW(static_cast<void>(
                   read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                         HEADING_TABLE_WITH_AN_UNKNOWN_SETTING, APPEARANCE})),
               UnknownDataFileKeyException);
}

/**
 * Given a straight body heading 2
 * When its gameplay is read
 * Then it is refused: a heading is -1 to 1
 */
TEST_F(EntitySheetReaderFixture, RefusesASettingOutOfItsRange) {
  EXPECT_THROW(
      static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                              TOO_FAR_HEADING_TABLE, APPEARANCE})),
      DataFileValueOutOfRangeException);
}

/**
 * Given a ship with an empty behaviour table
 * When its gameplay is read
 * Then it is refused: a player has no behaviour
 */
TEST_F(EntitySheetReaderFixture, RefusesABehaviourTableForAPlayer) {
  EXPECT_THROW(static_cast<void>(read({SHIP_GAMEPLAY, SHIP_HITBOX, SHIP_MOTION,
                                       EMPTY_BEHAVIOUR_TABLE, APPEARANCE})),
               InconsistentEntitySheetException);
}

/**
 * Given a Drifter of 0 health points
 * When its gameplay is read
 * Then it is refused: a body with health has at least 1 point
 */
TEST_F(EntitySheetReaderFixture, RefusesAHealthOfZero) {
  EXPECT_THROW(static_cast<void>(
                   read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                         LEFT_HEADING_TABLE, NO_HEALTH_TABLE, APPEARANCE})),
               DataFileValueOutOfRangeException);
}

/**
 * Given a Drifter whose gun names no projectile
 * When its gameplay is read
 * Then it is refused
 */
TEST_F(EntitySheetReaderFixture, RefusesAGunWithoutProjectile) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX,
                                       STEADY_MOTION, LEFT_HEADING_TABLE,
                                       GUN_WITHOUT_PROJECTILE, APPEARANCE})),
               MissingDataFileKeyException);
}

/**
 * Given a sheet of side enemy piloted by a player
 * When its gameplay is read
 * Then it is refused: only a ship has a player for pilot
 */
TEST_F(EntitySheetReaderFixture, RefusesAnEnemyPilotedByAPlayer) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_PILOTED_BY_A_PLAYER, BYDO_HITBOX,
                                       STEADY_MOTION, APPEARANCE})),
               InconsistentEntitySheetException);
}

/**
 * Given a sheet of side player piloted by the straight behaviour
 * When its gameplay is read
 * Then it is refused: a ship has its player for pilot
 */
TEST_F(EntitySheetReaderFixture, RefusesAPlayerPilotedByABehaviour) {
  EXPECT_THROW(
      static_cast<void>(read({PLAYER_PILOTED_BY_A_BEHAVIOUR, SHIP_HITBOX,
                              SHIP_MOTION, LEFT_HEADING_TABLE, APPEARANCE})),
      InconsistentEntitySheetException);
}

/**
 * Given a ship that moves steadily
 * When its gameplay is read
 * Then it is refused: the client predicts a ship with shipAfter()
 */
TEST_F(EntitySheetReaderFixture, RefusesAShipThatMovesSteadily) {
  EXPECT_THROW(static_cast<void>(read(
                   {SHIP_GAMEPLAY, SHIP_HITBOX, STEADY_MOTION, APPEARANCE})),
               InconsistentEntitySheetException);
}

/**
 * Given a Drifter that moves as a ship
 * When its gameplay is read
 * Then it is refused: only a ship moves as one
 */
TEST_F(EntitySheetReaderFixture, RefusesAnEnemyThatMovesAsAShip) {
  EXPECT_THROW(static_cast<void>(read({ENEMY_GAMEPLAY, BYDO_HITBOX, SHIP_MOTION,
                                       LEFT_HEADING_TABLE, APPEARANCE})),
               InconsistentEntitySheetException);
}

/**
 * Given a ship 120 units wide
 * When its gameplay is read
 * Then it is refused: shipAfter() keeps a box of 128 by 64 on screen
 */
TEST_F(EntitySheetReaderFixture, RefusesAShipOfAnotherBox) {
  EXPECT_THROW(static_cast<void>(read({SHIP_GAMEPLAY, NARROW_SHIP_HITBOX,
                                       SHIP_MOTION, APPEARANCE})),
               InconsistentEntitySheetException);
}

/**
 * Given a missile of a player with a gun
 * When its gameplay is read
 * Then it is refused: a missile fires nothing
 */
TEST_F(EntitySheetReaderFixture, RefusesAMissileWithAGun) {
  EXPECT_THROW(
      static_cast<void>(read({MISSILE_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                              LEFT_HEADING_TABLE, ENEMY_GUN, APPEARANCE})),
      InconsistentEntitySheetException);
}
