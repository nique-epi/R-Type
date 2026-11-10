#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BurstSheet.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetFixtures.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsFixtures.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "EntityType.hpp"
#include "EntityTypesTestConstants.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "MotionKind.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::engine::UnknownDataFileChoiceException;
using rtype::game::BurstSheet;
using rtype::game::DuplicateEntityTypeException;
using rtype::game::EntitySheet;
using rtype::game::EntitySheets;
using rtype::game::EntityType;
using rtype::game::InconsistentEntitySheetException;
using rtype::game::InvalidEntityTypeNameException;
using rtype::game::MotionKind;
using rtype::game::SheetText;
using rtype::game::TooManyEntityTypesException;
using rtype::game::UnknownEntityTypeException;
using rtype::game::testing::DRIFTER_FIRING_PLAYER_MISSILES_TEXT;
using rtype::game::testing::DRIFTER_NAME_IN_TESTS;
using rtype::game::testing::DRIFTER_NUMBER_AMONG_THREE;
using rtype::game::testing::DRIFTER_SHEET_TEXT;
using rtype::game::testing::DRIFTER_THAT_BURSTS_TEXT;
using rtype::game::testing::EMPTY_NAME;
using rtype::game::testing::ENEMY_MISSILE_NUMBER_AMONG_THREE;
using rtype::game::testing::ENEMY_MISSILE_SHEET;
using rtype::game::testing::ENEMY_MISSILE_SHEET_TEXT;
using rtype::game::testing::enemyMissileSheets;
using rtype::game::testing::firstShotTicksOf;
using rtype::game::testing::intervalTicksOf;
using rtype::game::testing::LAST_TYPE_NUMBER;
using rtype::game::testing::MOST_SHEETS_LOADED;
using rtype::game::testing::NAME_OF_24_BYTES;
using rtype::game::testing::NAME_OF_25_BYTES;
using rtype::game::testing::NAME_OF_NO_SHEET;
using rtype::game::testing::NAME_WITH_AN_UPPERCASE_LETTER;
using rtype::game::testing::namesIn;
using rtype::game::testing::NO_SHEET_NUMBER;
using rtype::game::testing::NUMBER_PAST_THE_THREE;
using rtype::game::testing::ONE_SHEET_TOO_MANY;
using rtype::game::testing::ORB_BURST_TICKS;
using rtype::game::testing::ORB_BURSTING_INTO_PLAYER_MISSILES_TEXT;
using rtype::game::testing::ORB_NAME_IN_TESTS;
using rtype::game::testing::ORB_SHEET_TEXT;
using rtype::game::testing::pilotBehaviourName;
using rtype::game::testing::PLAYER_MISSILE_NAME_IN_TESTS;
using rtype::game::testing::PLAYER_MISSILE_SHEET_TEXT;
using rtype::game::testing::projectileNameOf;
using rtype::game::testing::REQUIRED_BYDO_HEALTH;
using rtype::game::testing::REQUIRED_BYDO_SHOT_TICKS;
using rtype::game::testing::REQUIRED_BYDO_SIZE;
using rtype::game::testing::REQUIRED_BYDO_SPEED;
using rtype::game::testing::REQUIRED_DRIFTER_POINTS;
using rtype::game::testing::REQUIRED_ENEMY_MISSILE_SIZE;
using rtype::game::testing::REQUIRED_ENEMY_MISSILE_SPEED;
using rtype::game::testing::REQUIRED_LEFT_HEADING;
using rtype::game::testing::REQUIRED_PLAYER_MISSILE_HEIGHT;
using rtype::game::testing::REQUIRED_PLAYER_MISSILE_SPEED;
using rtype::game::testing::REQUIRED_PLAYER_MISSILE_WIDTH;
using rtype::game::testing::REQUIRED_RIGHT_HEADING;
using rtype::game::testing::REQUIRED_SHIP_FIRST_SHOT_TICKS;
using rtype::game::testing::REQUIRED_SHIP_HEIGHT;
using rtype::game::testing::REQUIRED_SHIP_SHOT_TICKS;
using rtype::game::testing::REQUIRED_SHIP_WIDTH;
using rtype::game::testing::REQUIRED_STRAIGHT_NAME;
using rtype::game::testing::REQUIRED_WEAVE;
using rtype::game::testing::REQUIRED_WEAVE_NAME;
using rtype::game::testing::REQUIRED_WEAVER_POINTS;
using rtype::game::testing::settingsOf;
using rtype::game::testing::SHARD_NAME_IN_TESTS;
using rtype::game::testing::SHARD_SHEET_TEXT;
using rtype::game::testing::SHARDS_PER_ORB;
using rtype::game::testing::sheetText;
using rtype::game::testing::SHIP_NAME_IN_TESTS;
using rtype::game::testing::SHIPPED_SHEET_NAMES;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::WEAVER_NAME_IN_TESTS;
using rtype::game::testing::WEAVER_NUMBER_AMONG_THREE;
using rtype::game::testing::WEAVER_SHEET_TEXT;
using ::testing::ElementsAreArray;

/**
 * Given the sheets of a Weaver, a Drifter and a missile of an enemy, in that
 * order
 * When they are read
 * Then they are numbered 1 for drifter, 2 for enemy_missile and 3 for weaver,
 * the order of their names
 */
TEST(EntitySheets, NumbersTheSheetsInTheOrderOfTheirNames) {
  const std::vector<SheetText> texts{
      sheetText(WEAVER_NAME_IN_TESTS, WEAVER_SHEET_TEXT),
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_SHEET_TEXT),
      sheetText(ENEMY_MISSILE_SHEET, ENEMY_MISSILE_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(sheets.named(DRIFTER_NAME_IN_TESTS).number,
            DRIFTER_NUMBER_AMONG_THREE);
  EXPECT_EQ(sheets.named(ENEMY_MISSILE_SHEET).number,
            ENEMY_MISSILE_NUMBER_AMONG_THREE);
  EXPECT_EQ(sheets.named(WEAVER_NAME_IN_TESTS).number,
            WEAVER_NUMBER_AMONG_THREE);
}

/**
 * Given the sheets of a Weaver, a Drifter and a missile of an enemy
 * When the third one is asked for by its number
 * Then it is the sheet of the Weaver
 */
TEST(EntitySheets, FindsASheetByItsNumber) {
  const std::vector<SheetText> texts{
      sheetText(WEAVER_NAME_IN_TESTS, WEAVER_SHEET_TEXT),
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_SHEET_TEXT),
      sheetText(ENEMY_MISSILE_SHEET, ENEMY_MISSILE_SHEET_TEXT)};
  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const EntitySheet* const third = sheets.numbered(WEAVER_NUMBER_AMONG_THREE);

  ASSERT_NE(third, nullptr);
  EXPECT_EQ(third->name, WEAVER_NAME_IN_TESTS);
}

/**
 * Given three sheets
 * When the sheet numbered 0 is asked for
 * Then there is none: numbers start at 1, and 0 names no type on the wire
 */
TEST(EntitySheets, NumberZeroNamesNoSheet) {
  const std::vector<SheetText> texts{
      sheetText(WEAVER_NAME_IN_TESTS, WEAVER_SHEET_TEXT),
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_SHEET_TEXT),
      sheetText(ENEMY_MISSILE_SHEET, ENEMY_MISSILE_SHEET_TEXT)};
  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(sheets.numbered(NO_SHEET_NUMBER), nullptr);
}

/**
 * Given three sheets
 * When the sheet numbered 4 is asked for
 * Then there is none
 */
TEST(EntitySheets, ANumberPastTheLastNamesNoSheet) {
  const std::vector<SheetText> texts{
      sheetText(WEAVER_NAME_IN_TESTS, WEAVER_SHEET_TEXT),
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_SHEET_TEXT),
      sheetText(ENEMY_MISSILE_SHEET, ENEMY_MISSILE_SHEET_TEXT)};
  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(sheets.numbered(NUMBER_PAST_THE_THREE), nullptr);
}

/**
 * Given the sheets of a Drifter and of a missile of an enemy
 * When a name no sheet has is asked for
 * Then it is refused
 */
TEST(EntitySheets, RefusesANameNoSheetHas) {
  const std::vector<SheetText> texts{
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_SHEET_TEXT),
      sheetText(ENEMY_MISSILE_SHEET, ENEMY_MISSILE_SHEET_TEXT)};
  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_THROW(static_cast<void>(sheets.named(NAME_OF_NO_SHEET)),
               UnknownEntityTypeException);
}

/**
 * Given the sheets of a Drifter, whose gun fires enemy_missile, and of that
 * missile
 * When the sheet numbered as what the gun of the Drifter fires is looked up
 * Then it is the sheet of the missile of an enemy
 */
TEST(EntitySheets, NumbersWhatAGunFires) {
  const std::vector<SheetText> texts{
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_SHEET_TEXT),
      sheetText(ENEMY_MISSILE_SHEET, ENEMY_MISSILE_SHEET_TEXT)};
  const EntitySheets sheets = EntitySheets::fromTexts(texts);
  const EntitySheet& drifter = sheets.named(DRIFTER_NAME_IN_TESTS);
  ASSERT_TRUE(drifter.gun.has_value());

  const EntitySheet* const projectile = sheets.numbered(
      drifter.gun.value_or(rtype::game::GunSheet{}).projectileNumber);

  ASSERT_NE(projectile, nullptr);
  EXPECT_EQ(projectile->name, ENEMY_MISSILE_SHEET);
}

/**
 * Given the sheet of a Drifter, whose gun fires enemy_missile, without that
 * sheet
 * When the sheets are read
 * Then they are refused: a gun fires a type that exists
 */
TEST(EntitySheets, RefusesAGunThatFiresNoSheet) {
  const std::vector<SheetText> texts{
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               UnknownDataFileChoiceException);
}

/**
 * Given the sheet of a Drifter whose gun fires the missile of a player
 * When the sheets are read
 * Then they are refused: an enemy fires missiles of an enemy
 */
TEST(EntitySheets, RefusesAnEnemyThatFiresTheMissileOfAPlayer) {
  const std::vector<SheetText> texts{
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_FIRING_PLAYER_MISSILES_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given a sheet named Drifter, with an uppercase letter
 * When the sheets are read
 * Then they are refused: a name is the same file on every system
 */
TEST(EntitySheets, RefusesANameWithAnUppercaseLetter) {
  const std::vector<SheetText> texts{
      sheetText(NAME_WITH_AN_UPPERCASE_LETTER, ENEMY_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InvalidEntityTypeNameException);
}

/**
 * Given a sheet with an empty name
 * When the sheets are read
 * Then they are refused
 */
TEST(EntitySheets, RefusesAnEmptyName) {
  const std::vector<SheetText> texts{
      sheetText(EMPTY_NAME, ENEMY_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InvalidEntityTypeNameException);
}

/**
 * Given a sheet whose name is 24 bytes long, the longest a name may be
 * When the sheets are read
 * Then the sheet is read under that name
 */
TEST(EntitySheets, AcceptsANameOf24Bytes) {
  const std::vector<SheetText> texts{
      sheetText(NAME_OF_24_BYTES, ENEMY_MISSILE_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(namesIn(sheets),
            (std::vector<std::string>{std::string{NAME_OF_24_BYTES}}));
}

/**
 * Given a sheet whose name is 25 bytes long
 * When the sheets are read
 * Then they are refused
 */
TEST(EntitySheets, RefusesANameOf25Bytes) {
  const std::vector<SheetText> texts{
      sheetText(NAME_OF_25_BYTES, ENEMY_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InvalidEntityTypeNameException);
}

/**
 * Given 255 sheets, the most a program loads
 * When they are read
 * Then all 255 are read
 */
TEST(EntitySheets, ReadsTheMostSheetsAProgramLoads) {
  const std::vector<SheetText> texts = enemyMissileSheets(MOST_SHEETS_LOADED);

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(sheets.all().size(), MOST_SHEETS_LOADED);
}

/**
 * Given 255 sheets, the most a program loads
 * When the sheet numbered 255 is asked for
 * Then it is the last sheet in the order of the names: the last number a
 * byte holds names a type
 */
TEST(EntitySheets, TheLastSheetTakesTheLastTypeNumber) {
  const std::vector<SheetText> texts = enemyMissileSheets(MOST_SHEETS_LOADED);
  const std::string lastName =
      std::ranges::max(texts, std::less{}, &SheetText::name).name;
  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const EntitySheet* const last = sheets.numbered(LAST_TYPE_NUMBER);

  ASSERT_NE(last, nullptr);
  EXPECT_EQ(last->name, lastName);
}

/**
 * Given 256 sheets
 * When they are read
 * Then they are refused: a type number is one byte, and 0 names none
 */
TEST(EntitySheets, RefusesOneSheetPastTheMost) {
  const std::vector<SheetText> texts = enemyMissileSheets(ONE_SHEET_TOO_MANY);

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               TooManyEntityTypesException);
}

/**
 * Given two sheets named drifter
 * When they are read
 * Then they are refused: a name finds one type
 */
TEST(EntitySheets, RefusesTwoSheetsOfTheSameName) {
  const std::vector<SheetText> texts{
      sheetText(DRIFTER_NAME_IN_TESTS, ENEMY_MISSILE_SHEET_TEXT),
      sheetText(DRIFTER_NAME_IN_TESTS, ENEMY_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               DuplicateEntityTypeException);
}

/**
 * Given the sheets of a Weaver and of a missile of an enemy, in that order
 * When they are read
 * Then the texts are kept in the order of the names, the missile first, each
 * as it was given
 */
TEST(EntitySheets, KeepsTheTextsInTheOrderOfTheNames) {
  const std::vector<SheetText> texts{
      sheetText(WEAVER_NAME_IN_TESTS, WEAVER_SHEET_TEXT),
      sheetText(ENEMY_MISSILE_SHEET, ENEMY_MISSILE_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  ASSERT_EQ(sheets.texts().size(), texts.size());
  EXPECT_EQ(sheets.texts().front().text, ENEMY_MISSILE_SHEET_TEXT);
  EXPECT_EQ(sheets.texts().back().text, WEAVER_SHEET_TEXT);
}

/**
 * Given the sheets of a Weaver and of a missile of an enemy, moved into
 * another catalog once read
 * When the pilot of the Weaver is read in the new catalog
 * Then it is still the weave, with its amplitude of 48 and its period of
 * 2000 ms: moving the catalog moves no behaviour and no setting
 */
TEST(EntitySheets, APilotStillHoldsAfterTheSheetsAreMoved) {
  const std::vector<SheetText> texts{
      sheetText(WEAVER_NAME_IN_TESTS, WEAVER_SHEET_TEXT),
      sheetText(ENEMY_MISSILE_SHEET, ENEMY_MISSILE_SHEET_TEXT)};
  EntitySheets read = EntitySheets::fromTexts(texts);

  const EntitySheets moved{std::move(read)};

  const EntitySheet& weaver = moved.named(WEAVER_NAME_IN_TESTS);
  EXPECT_EQ(pilotBehaviourName(weaver), REQUIRED_WEAVE_NAME);
  EXPECT_THAT(settingsOf(weaver), ElementsAreArray(REQUIRED_WEAVE));
}

/**
 * Given the folder of the sheets the game ships with
 * When its sheets are read
 * Then they are the types of today, in the order of their names (see
 * SHIPPED_SHEET_NAMES)
 */
TEST(EntitySheets, TheShippedSheetsAreTheTypesOfToday) {
  const EntitySheets& sheets = shippedEntitySheets();

  EXPECT_THAT(namesIn(sheets), ElementsAreArray(SHIPPED_SHEET_NAMES));
}

/**
 * Given the shipped sheet of the Drifter
 * When it is read
 * Then it is today's Drifter: an enemy of 96 by 96 that heads left on its row
 * at 240, has 3 health points, is worth 100 points, and fires a missile of an
 * enemy every 90 ticks, the first one 90 ticks after it appeared
 */
TEST(EntitySheets, TheShippedDrifterIsTodaysDrifter) {
  const EntitySheets& sheets = shippedEntitySheets();

  const EntitySheet& drifter = sheets.named(DRIFTER_NAME_IN_TESTS);

  EXPECT_EQ(drifter.side, EntityType::Enemy);
  EXPECT_FLOAT_EQ(drifter.hitbox.width, REQUIRED_BYDO_SIZE);
  EXPECT_FLOAT_EQ(drifter.hitbox.height, REQUIRED_BYDO_SIZE);
  EXPECT_EQ(drifter.motion.kind, MotionKind::Steady);
  EXPECT_FLOAT_EQ(drifter.motion.topSpeed, REQUIRED_BYDO_SPEED);
  EXPECT_EQ(pilotBehaviourName(drifter), REQUIRED_STRAIGHT_NAME);
  EXPECT_THAT(settingsOf(drifter), ElementsAreArray(REQUIRED_LEFT_HEADING));
  EXPECT_EQ(drifter.health.value_or(0), REQUIRED_BYDO_HEALTH);
  EXPECT_EQ(drifter.points.value_or(0U), REQUIRED_DRIFTER_POINTS);
  EXPECT_EQ(intervalTicksOf(drifter), REQUIRED_BYDO_SHOT_TICKS);
  EXPECT_EQ(firstShotTicksOf(drifter), REQUIRED_BYDO_SHOT_TICKS);
  EXPECT_EQ(projectileNameOf(drifter), ENEMY_MISSILE_SHEET);
}

/**
 * Given the shipped sheet of the Weaver
 * When it is read
 * Then it is today's Weaver: a Drifter that weaves 48 units every 2000 ms and
 * is worth 200 points
 */
TEST(EntitySheets, TheShippedWeaverIsTodaysWeaver) {
  const EntitySheets& sheets = shippedEntitySheets();

  const EntitySheet& weaver = sheets.named(WEAVER_NAME_IN_TESTS);

  EXPECT_EQ(weaver.side, EntityType::Enemy);
  EXPECT_FLOAT_EQ(weaver.hitbox.width, REQUIRED_BYDO_SIZE);
  EXPECT_FLOAT_EQ(weaver.hitbox.height, REQUIRED_BYDO_SIZE);
  EXPECT_FLOAT_EQ(weaver.motion.topSpeed, REQUIRED_BYDO_SPEED);
  EXPECT_EQ(pilotBehaviourName(weaver), REQUIRED_WEAVE_NAME);
  EXPECT_THAT(settingsOf(weaver), ElementsAreArray(REQUIRED_WEAVE));
  EXPECT_EQ(weaver.health.value_or(0), REQUIRED_BYDO_HEALTH);
  EXPECT_EQ(weaver.points.value_or(0U), REQUIRED_WEAVER_POINTS);
  EXPECT_EQ(intervalTicksOf(weaver), REQUIRED_BYDO_SHOT_TICKS);
  EXPECT_EQ(projectileNameOf(weaver), ENEMY_MISSILE_SHEET);
}

/**
 * Given the shipped sheet of the ship
 * When it is read
 * Then it is today's ship: a body of side player of 128 by 64, moved as a ship
 * and piloted by its player, that fires a missile of a player every 20 ticks
 * and at once
 */
TEST(EntitySheets, TheShippedShipIsTodaysShip) {
  const EntitySheets& sheets = shippedEntitySheets();

  const EntitySheet& ship = sheets.named(SHIP_NAME_IN_TESTS);

  EXPECT_EQ(ship.side, EntityType::Player);
  EXPECT_FLOAT_EQ(ship.hitbox.width, REQUIRED_SHIP_WIDTH);
  EXPECT_FLOAT_EQ(ship.hitbox.height, REQUIRED_SHIP_HEIGHT);
  EXPECT_EQ(ship.motion.kind, MotionKind::Ship);
  EXPECT_FALSE(ship.pilot.has_value());
  EXPECT_EQ(intervalTicksOf(ship), REQUIRED_SHIP_SHOT_TICKS);
  EXPECT_EQ(firstShotTicksOf(ship), REQUIRED_SHIP_FIRST_SHOT_TICKS);
  EXPECT_EQ(projectileNameOf(ship), PLAYER_MISSILE_NAME_IN_TESTS);
}

/**
 * Given the shipped sheet of the missile of a player
 * When it is read
 * Then it is today's missile: 64 by 24, heading right at 960, with no gun
 */
TEST(EntitySheets, TheShippedMissileOfAPlayerIsTodaysMissile) {
  const EntitySheets& sheets = shippedEntitySheets();

  const EntitySheet& missile = sheets.named(PLAYER_MISSILE_NAME_IN_TESTS);

  EXPECT_EQ(missile.side, EntityType::PlayerMissile);
  EXPECT_FLOAT_EQ(missile.hitbox.width, REQUIRED_PLAYER_MISSILE_WIDTH);
  EXPECT_FLOAT_EQ(missile.hitbox.height, REQUIRED_PLAYER_MISSILE_HEIGHT);
  EXPECT_FLOAT_EQ(missile.motion.topSpeed, REQUIRED_PLAYER_MISSILE_SPEED);
  EXPECT_EQ(pilotBehaviourName(missile), REQUIRED_STRAIGHT_NAME);
  EXPECT_THAT(settingsOf(missile), ElementsAreArray(REQUIRED_RIGHT_HEADING));
  EXPECT_FALSE(missile.gun.has_value());
}

/**
 * Given the shipped sheet of the missile of an enemy
 * When it is read
 * Then it is today's missile: 32 by 32, heading left at 360, with no gun
 */
TEST(EntitySheets, TheShippedMissileOfAnEnemyIsTodaysMissile) {
  const EntitySheets& sheets = shippedEntitySheets();

  const EntitySheet& missile = sheets.named(ENEMY_MISSILE_SHEET);

  EXPECT_EQ(missile.side, EntityType::EnemyMissile);
  EXPECT_FLOAT_EQ(missile.hitbox.width, REQUIRED_ENEMY_MISSILE_SIZE);
  EXPECT_FLOAT_EQ(missile.hitbox.height, REQUIRED_ENEMY_MISSILE_SIZE);
  EXPECT_FLOAT_EQ(missile.motion.topSpeed, REQUIRED_ENEMY_MISSILE_SPEED);
  EXPECT_EQ(pilotBehaviourName(missile), REQUIRED_STRAIGHT_NAME);
  EXPECT_THAT(settingsOf(missile), ElementsAreArray(REQUIRED_LEFT_HEADING));
  EXPECT_FALSE(missile.gun.has_value());
}

/**
 * Given the sheets of an orb that bursts into `shard`, and of that shard
 * When the sheet numbered as the shards of the orb is looked up
 * Then it is the sheet of the shard
 */
TEST(EntitySheets, NumbersTheShardsOfABurst) {
  const std::vector<SheetText> texts{
      sheetText(ORB_NAME_IN_TESTS, ORB_SHEET_TEXT),
      sheetText(SHARD_NAME_IN_TESTS, SHARD_SHEET_TEXT)};
  const EntitySheets sheets = EntitySheets::fromTexts(texts);
  const EntitySheet& orb = sheets.named(ORB_NAME_IN_TESTS);
  ASSERT_TRUE(orb.burst.has_value());

  const EntitySheet* const shard =
      sheets.numbered(orb.burst.value_or(BurstSheet{}).shardNumber);

  ASSERT_NE(shard, nullptr);
  EXPECT_EQ(shard->name, SHARD_NAME_IN_TESTS);
}

/**
 * Given the sheet of an orb that bursts after 100 ms into four shards
 * When the sheets are read
 * Then it bursts after 6 ticks into 4 shards
 */
TEST(EntitySheets, ReadsWhenAndIntoHowManyShardsAMissileBursts) {
  const std::vector<SheetText> texts{
      sheetText(ORB_NAME_IN_TESTS, ORB_SHEET_TEXT),
      sheetText(SHARD_NAME_IN_TESTS, SHARD_SHEET_TEXT)};
  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  const std::optional<BurstSheet> burst = sheets.named(ORB_NAME_IN_TESTS).burst;

  ASSERT_TRUE(burst.has_value());
  EXPECT_EQ(burst.value_or(BurstSheet{}).afterTicks, ORB_BURST_TICKS);
  EXPECT_EQ(burst.value_or(BurstSheet{}).count, SHARDS_PER_ORB);
}

/**
 * Given the sheet of an orb that bursts into `shard`, without that sheet
 * When the sheets are read
 * Then they are refused: a missile bursts into a type that exists
 */
TEST(EntitySheets, RefusesABurstIntoNoSheet) {
  const std::vector<SheetText> texts{
      sheetText(ORB_NAME_IN_TESTS, ORB_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               UnknownDataFileChoiceException);
}

/**
 * Given the sheet of a missile of an enemy that bursts into missiles of a
 * player
 * When the sheets are read
 * Then they are refused: a missile bursts into missiles of its own side
 */
TEST(EntitySheets, RefusesAMissileThatBurstsIntoAnotherSide) {
  const std::vector<SheetText> texts{
      sheetText(ORB_NAME_IN_TESTS, ORB_BURSTING_INTO_PLAYER_MISSILES_TEXT),
      sheetText(PLAYER_MISSILE_NAME_IN_TESTS, PLAYER_MISSILE_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}

/**
 * Given the sheet of a Drifter that declares a burst
 * When the sheets are read
 * Then they are refused: only a missile bursts
 */
TEST(EntitySheets, RefusesAnEnemyThatBursts) {
  const std::vector<SheetText> texts{
      sheetText(DRIFTER_NAME_IN_TESTS, DRIFTER_THAT_BURSTS_TEXT),
      sheetText(SHARD_NAME_IN_TESTS, SHARD_SHEET_TEXT)};

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               InconsistentEntitySheetException);
}
