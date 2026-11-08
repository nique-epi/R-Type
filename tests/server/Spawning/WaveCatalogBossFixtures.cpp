#include "WaveCatalogBossFixtures.hpp"
#include <vector>
#include "BossStageTestConstants.hpp"
#include "EntitySheets.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"
#include "ShippedEntitySheets.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The sheets the game ships and the warlord. */
game::EntitySheets readSheetsWithAWarlord() {
  std::vector<game::SheetText> texts{
      game::testing::shippedEntitySheets().texts().begin(),
      game::testing::shippedEntitySheets().texts().end()};
  texts.push_back(
      game::testing::sheetText(WARLORD_NAME_IN_TESTS, WARLORD_SHEET_TEXT));
  return game::EntitySheets::fromTexts(texts);
}

}  // namespace

const game::EntitySheets& sheetsWithAWarlord() {
  static const game::EntitySheets sheets = readSheetsWithAWarlord();
  return sheets;
}

}  // namespace rtype::server::testing
