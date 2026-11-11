#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetCollisionTestConstants.hpp"
#include "EntitySheetFixtures.hpp"
#include "EntitySheetReaderFixture.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "GameException.hpp"

using rtype::engine::DataFileTextsExpectedException;
using rtype::engine::MissingDataFileKeyException;
using rtype::game::EntitySheet;
using rtype::game::InvalidCollisionLayerNameException;
using rtype::game::testing::APPEARANCE;
using rtype::game::testing::BYDO_HITBOX;
using rtype::game::testing::ENEMY_GAMEPLAY;
using rtype::game::testing::ENEMY_HITTING_SHIPS_AND_WALLS;
using rtype::game::testing::ENEMY_IN_A_LAYER_WITH_AN_UPPERCASE_LETTER;
using rtype::game::testing::ENEMY_IN_A_LAYER_WITHOUT_NAME;
using rtype::game::testing::ENEMY_LAYER_NAME;
using rtype::game::testing::ENEMY_WHOSE_HITS_ARE_ONE_TEXT;
using rtype::game::testing::ENEMY_WITHOUT_LAYER;
using rtype::game::testing::EntitySheetReaderFixture;
using rtype::game::testing::hitLayersOf;
using rtype::game::testing::LAYER_NAME_REFUSAL;
using rtype::game::testing::LEFT_HEADING_TABLE;
using rtype::game::testing::PLAYER_LAYER_NAME;
using rtype::game::testing::STEADY_MOTION;
using rtype::game::testing::WALL_LAYER_NAME;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given the sheet of an enemy whose layer is "enemy"
 * When its gameplay is read
 * Then it lies in the layer named enemy
 */
TEST_F(EntitySheetReaderFixture, ReadsTheLayerOfTheSheet) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_EQ(sheet.collision.layer, ENEMY_LAYER_NAME);
}

/**
 * Given the sheet of an enemy that hits the layers player then wall
 * When its gameplay is read
 * Then it hits player then wall, in the order of the file
 */
TEST_F(EntitySheetReaderFixture, ReadsTheLayersItHitsInTheOrderOfTheFile) {
  const EntitySheet sheet =
      read({ENEMY_HITTING_SHIPS_AND_WALLS, BYDO_HITBOX, STEADY_MOTION,
            LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_THAT(hitLayersOf(sheet),
              ElementsAre(PLAYER_LAYER_NAME, WALL_LAYER_NAME));
}

/**
 * Given the sheet of an enemy with no key hits
 * When its gameplay is read
 * Then it hits no layer
 */
TEST_F(EntitySheetReaderFixture, ASheetWithoutHitsHitsNothing) {
  const EntitySheet sheet = read({ENEMY_GAMEPLAY, BYDO_HITBOX, STEADY_MOTION,
                                  LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_THAT(hitLayersOf(sheet), IsEmpty());
}

/**
 * Given the sheet of an enemy with no key layer
 * When its gameplay is read
 * Then a MissingDataFileKeyException is thrown: every body lies in a layer
 */
TEST_F(EntitySheetReaderFixture, RefusesASheetWithoutLayer) {
  EXPECT_THROW(
      static_cast<void>(read({ENEMY_WITHOUT_LAYER, BYDO_HITBOX, STEADY_MOTION,
                              LEFT_HEADING_TABLE, APPEARANCE})),
      MissingDataFileKeyException);
}

/**
 * Given the sheet of an enemy whose layer is "Enemy", with an uppercase
 * letter
 * When its gameplay is read
 * Then the refusal names the file, the line and the column of the layer, its
 * key, and how a layer is named
 */
TEST_F(EntitySheetReaderFixture, RefusesALayerWithAnUppercaseLetter) {
  const std::string message =
      refusalOf({ENEMY_IN_A_LAYER_WITH_AN_UPPERCASE_LETTER, BYDO_HITBOX,
                 STEADY_MOTION, LEFT_HEADING_TABLE, APPEARANCE});

  EXPECT_EQ(message, LAYER_NAME_REFUSAL);
}

/**
 * Given the sheet of an enemy whose layer is the empty text
 * When its gameplay is read
 * Then an InvalidCollisionLayerNameException is thrown
 */
TEST_F(EntitySheetReaderFixture, RefusesALayerWithoutName) {
  EXPECT_THROW(
      static_cast<void>(read({ENEMY_IN_A_LAYER_WITHOUT_NAME, BYDO_HITBOX,
                              STEADY_MOTION, LEFT_HEADING_TABLE, APPEARANCE})),
      InvalidCollisionLayerNameException);
}

/**
 * Given the sheet of an enemy whose hits are one text, not an array
 * When its gameplay is read
 * Then a DataFileTextsExpectedException is thrown
 */
TEST_F(EntitySheetReaderFixture, RefusesHitsThatAreNotAnArray) {
  EXPECT_THROW(
      static_cast<void>(read({ENEMY_WHOSE_HITS_ARE_ONE_TEXT, BYDO_HITBOX,
                              STEADY_MOTION, LEFT_HEADING_TABLE, APPEARANCE})),
      DataFileTextsExpectedException);
}
