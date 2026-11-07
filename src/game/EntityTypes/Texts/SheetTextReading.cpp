#include "SheetTextReading.hpp"
#include <filesystem>
#include <span>
#include <string_view>
#include <vector>
#include "CollisionBox.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeName.hpp"
#include "GameException.hpp"
#include "PathText.hpp"
#include "SheetText.hpp"

namespace rtype::game {

std::vector<SheetText> sheetTextsIn(const std::filesystem::path& folder) {
  const std::vector<std::filesystem::path> files = engine::dataFilesIn(folder);
  if (files.empty()) {
    throw NoEntitySheetException(engine::shownPath(folder));
  }
  std::vector<SheetText> texts;
  texts.reserve(files.size());
  for (const std::filesystem::path& file : files) {
    texts.push_back(SheetText{.name = logging::genericText(file.stem()),
                              .source = engine::shownPath(file),
                              .text = engine::dataFileContent(file)});
  }
  checkSheetNames(texts);
  return texts;
}

void checkSheetNames(std::span<const SheetText> texts) {
  if (texts.size() > identifiers::MAXIMUM_ENTITY_TYPES) {
    throw TooManyEntityTypesException(texts.size(),
                                      identifiers::MAXIMUM_ENTITY_TYPES);
  }
  for (const SheetText& text : texts) {
    if (!identifiers::isEntityTypeName(text.name)) {
      throw InvalidEntityTypeNameException(
          text.source, identifiers::MAXIMUM_ENTITY_TYPE_NAME_LENGTH);
    }
  }
}

engine::DataFileTable sheetTableOf(const engine::DataFileTable& root,
                                   std::string_view key) {
  engine::refuseUnknownKeys(root, SHEET_ROOT_KEYS);
  for (const std::string_view other : SHEET_ROOT_KEYS) {
    if (other != key) {
      static_cast<void>(engine::tableAt(root, other));
    }
  }
  return engine::tableAt(root, key);
}

CollisionBox hitboxOf(const engine::DataFileTable& gameplay) {
  const engine::DataFileTable hitbox = engine::tableAt(gameplay, HITBOX_KEY);
  engine::refuseUnknownKeys(hitbox, HITBOX_KEYS);
  return CollisionBox{.width = static_cast<float>(engine::wholeNumberAt(
                          hitbox, WIDTH_KEY, HITBOX_SIDE_RANGE)),
                      .height = static_cast<float>(engine::wholeNumberAt(
                          hitbox, HEIGHT_KEY, HITBOX_SIDE_RANGE))};
}

}  // namespace rtype::game
