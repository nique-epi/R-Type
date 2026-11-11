#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EngineException.hpp"
#include "EntitySheetCollisionTestConstants.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsFixtures.hpp"
#include "GameException.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"

using rtype::engine::UnknownDataFileChoiceException;
using rtype::game::EntitySheets;
using rtype::game::SheetText;
using rtype::game::TooManyCollisionLayersException;
using rtype::game::testing::BUG_LAYER_SET;
using rtype::game::testing::BUG_SHEET_NAME;
using rtype::game::testing::BUG_SHEET_TEXT;
using rtype::game::testing::MOST_LAYERS_NAMED;
using rtype::game::testing::NO_LAYER_SET;
using rtype::game::testing::ONE_LAYER_TOO_MANY;
using rtype::game::testing::OTHER_BUG_SHEET_NAME;
using rtype::game::testing::ROCK_SHEET_NAME;
using rtype::game::testing::ROCK_SHEET_TEXT;
using rtype::game::testing::sheetsOfDistinctLayers;
using rtype::game::testing::sheetText;
using rtype::game::testing::SHOT_LAYER_SET;
using rtype::game::testing::SHOT_SHEET_NAME;
using rtype::game::testing::SHOT_SHEET_TEXT;
using rtype::game::testing::UNKNOWN_HIT_REFUSAL;
using rtype::game::testing::WALL_LAYER_SET;
using rtype::game::testing::WALLS_AND_BUGS_SET;
using ::testing::StrEq;
using ::testing::ThrowsMessage;

/**
 * Given a rock in the layer wall, a shot in the layer shot and a bug in the
 * layer bug
 * When the catalog reads them
 * Then each layer has one bit, in the order of the names: bug, shot, wall
 */
TEST(EntitySheets, GivesEachLayerABitInTheOrderOfTheNames) {
  const std::vector<SheetText> texts{
      sheetText(ROCK_SHEET_NAME, ROCK_SHEET_TEXT),
      sheetText(SHOT_SHEET_NAME, SHOT_SHEET_TEXT),
      sheetText(BUG_SHEET_NAME, BUG_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(sheets.named(BUG_SHEET_NAME).collision.filter.layer, BUG_LAYER_SET);
  EXPECT_EQ(sheets.named(SHOT_SHEET_NAME).collision.filter.layer,
            SHOT_LAYER_SET);
  EXPECT_EQ(sheets.named(ROCK_SHEET_NAME).collision.filter.layer,
            WALL_LAYER_SET);
}

/**
 * Given a shot that hits the walls and the bugs, a rock that hits the shots
 * and a bug that hits nothing
 * When the catalog reads them
 * Then the filter of each hits exactly the layers its sheet names
 */
TEST(EntitySheets, ASheetHitsTheLayersItNames) {
  const std::vector<SheetText> texts{
      sheetText(ROCK_SHEET_NAME, ROCK_SHEET_TEXT),
      sheetText(SHOT_SHEET_NAME, SHOT_SHEET_TEXT),
      sheetText(BUG_SHEET_NAME, BUG_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(sheets.named(SHOT_SHEET_NAME).collision.filter.hits,
            WALLS_AND_BUGS_SET);
  EXPECT_EQ(sheets.named(ROCK_SHEET_NAME).collision.filter.hits,
            SHOT_LAYER_SET);
  EXPECT_EQ(sheets.named(BUG_SHEET_NAME).collision.filter.hits, NO_LAYER_SET);
}

/**
 * Given two sheets that both lie in the layer bug
 * When the catalog reads them
 * Then they share the bit of that layer
 */
TEST(EntitySheets, TwoSheetsOfOneLayerShareItsBit) {
  const std::vector<SheetText> texts{
      sheetText(BUG_SHEET_NAME, BUG_SHEET_TEXT),
      sheetText(OTHER_BUG_SHEET_NAME, BUG_SHEET_TEXT)};

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(sheets.named(BUG_SHEET_NAME).collision.filter.layer,
            sheets.named(OTHER_BUG_SHEET_NAME).collision.filter.layer);
}

/**
 * Given a shot that hits the layer wall, and no sheet in that layer
 * When the catalog reads the sheets
 * Then the refusal names the file, the line and the column of "wall", the key
 * and the layers the sheets lie in
 */
TEST(EntitySheets, RefusesAHitOnALayerNoSheetLiesIn) {
  const std::vector<SheetText> texts{
      sheetText(SHOT_SHEET_NAME, SHOT_SHEET_TEXT),
      sheetText(BUG_SHEET_NAME, BUG_SHEET_TEXT)};

  EXPECT_THAT([&texts] { static_cast<void>(EntitySheets::fromTexts(texts)); },
              ThrowsMessage<UnknownDataFileChoiceException>(
                  StrEq(std::string{UNKNOWN_HIT_REFUSAL})));
}

/**
 * Given 32 sheets, each in a layer of its own
 * When the catalog reads them
 * Then it reads the 32: a filter tells 32 layers apart
 */
TEST(EntitySheets, ReadsAsManyLayersAsAFilterTellsApart) {
  const std::vector<SheetText> texts =
      sheetsOfDistinctLayers(MOST_LAYERS_NAMED);

  const EntitySheets sheets = EntitySheets::fromTexts(texts);

  EXPECT_EQ(sheets.all().size(), MOST_LAYERS_NAMED);
}

/**
 * Given 33 sheets, each in a layer of its own
 * When the catalog reads them
 * Then a TooManyCollisionLayersException is thrown
 */
TEST(EntitySheets, RefusesOneLayerTooMany) {
  const std::vector<SheetText> texts =
      sheetsOfDistinctLayers(ONE_LAYER_TOO_MANY);

  EXPECT_THROW(static_cast<void>(EntitySheets::fromTexts(texts)),
               TooManyCollisionLayersException);
}
