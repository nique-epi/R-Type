#include "NumberedEntityTypes.hpp"
#include <algorithm>
#include <functional>
#include <span>
#include <toml++/toml.hpp>
#include <vector>
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityType.hpp"
#include "NumberedEntityType.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"

namespace rtype::game {

namespace {

/** @returns The side the `[gameplay]` of @p sheet names. */
EntityType sideOf(const SheetText& sheet) {
  const toml::table root = engine::parseDataFile(sheet.text, sheet.source);
  const engine::DataFileTable gameplay =
      sheetTableOf(engine::rootOf(root, sheet.source), GAMEPLAY_KEY);
  return SIDES.at(engine::choiceAt(gameplay, SIDE_KEY, SIDE_NAMES));
}

}  // namespace

std::vector<NumberedEntityType> numberedEntityTypesOf(
    std::span<const SheetText> sheets) {
  checkSheetNames(sheets);
  std::vector<NumberedEntityType> types;
  types.reserve(sheets.size());
  for (const SheetText& sheet : sheets) {
    types.push_back(
        NumberedEntityType{.name = sheet.name, .side = sideOf(sheet)});
  }
  std::ranges::sort(types, std::less{}, &NumberedEntityType::name);
  return types;
}

}  // namespace rtype::game
