#include <gtest/gtest.h>
#include <vector>
#include "EntitySheets.hpp"
#include "EntitySheetsFingerprint.hpp"
#include "EntitySheetsFingerprintTestConstants.hpp"
#include "ServerConstants.hpp"
#include "SheetText.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::game::EntitySheets;
using rtype::game::SheetText;
using rtype::game::testing::shippedEntitySheets;
using rtype::server::entitySheetsFingerprint;
using rtype::server::OFFICIAL_ENTITY_SHEETS_FINGERPRINT;
using rtype::server::testing::A_COMMENT_LINE_OF_A_SHEET;

/**
 * Given the entity sheets the game ships
 * When their fingerprint is taken
 * Then it is OFFICIAL_ENTITY_SHEETS_FINGERPRINT, so a change of a shipped sheet
 * fails here until the constant follows it
 */
TEST(EntitySheetsFingerprint, TheShippedSheetsHaveTheOfficialFingerprint) {
  EXPECT_EQ(entitySheetsFingerprint(shippedEntitySheets()),
            OFFICIAL_ENTITY_SHEETS_FINGERPRINT);
}

/**
 * Given the shipped sheets, and a catalog read again from the same texts
 * When both fingerprints are taken
 * Then they are equal
 */
TEST(EntitySheetsFingerprint, TheSameTextsGiveTheSameFingerprint) {
  const EntitySheets again =
      EntitySheets::fromTexts(shippedEntitySheets().texts());

  EXPECT_EQ(entitySheetsFingerprint(again),
            entitySheetsFingerprint(shippedEntitySheets()));
}

/**
 * Given the shipped sheets, and the same with a comment line added to one
 * When both fingerprints are taken
 * Then they differ: a comment changes the text of a sheet
 */
TEST(EntitySheetsFingerprint, ACommentAddedToASheetChangesIt) {
  std::vector<SheetText> texts(shippedEntitySheets().texts().begin(),
                               shippedEntitySheets().texts().end());
  texts.front().text += A_COMMENT_LINE_OF_A_SHEET;

  const EntitySheets changed = EntitySheets::fromTexts(texts);

  EXPECT_NE(entitySheetsFingerprint(changed),
            entitySheetsFingerprint(shippedEntitySheets()));
}
