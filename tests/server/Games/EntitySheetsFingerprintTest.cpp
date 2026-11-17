#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include "EntitySheetConstants.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsFingerprint.hpp"
#include "EntitySheetsFingerprintTestConstants.hpp"
#include "ServerConstants.hpp"
#include "SheetMaskText.hpp"
#include "SheetText.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::game::CLEAR_MASK_PIXEL;
using rtype::game::DRAWN_MASK_PIXEL;
using rtype::game::EntitySheets;
using rtype::game::SheetMaskText;
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
 * Given the shipped sheets, and the same with one more pixel drawn in the
 * first mask file
 * When both fingerprints are taken
 * Then they differ: a server whose mask files were changed is no longer
 * read as one that plays the shipped sheets
 */
TEST(EntitySheetsFingerprint, APixelDrawnInAMaskFileChangesIt) {
  std::vector<SheetText> texts(shippedEntitySheets().texts().begin(),
                               shippedEntitySheets().texts().end());
  const auto masked = std::ranges::find_if(
      texts,
      [](const SheetText& text) noexcept { return text.mask.has_value(); });
  ASSERT_NE(masked, texts.end());
  SheetMaskText mask = masked->mask.value_or(SheetMaskText{});
  mask.text[mask.text.find(CLEAR_MASK_PIXEL)] = DRAWN_MASK_PIXEL;
  masked->mask = mask;

  const EntitySheets changed = EntitySheets::fromTexts(texts);

  EXPECT_NE(entitySheetsFingerprint(changed),
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
