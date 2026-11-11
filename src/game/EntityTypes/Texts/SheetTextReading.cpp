#include "SheetTextReading.hpp"
#include <filesystem>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
#include "CollisionBox.hpp"
#include "ContentName.hpp"
#include "ContentNameConstants.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityTypeConstants.hpp"
#include "GameException.hpp"
#include "PathText.hpp"
#include "SheetText.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

namespace {

/** @returns The value of @p key of @p table as a number of logical units. */
float unitsAt(const engine::DataFileTable& table, std::string_view key,
              engine::WholeNumberRange range) {
  return static_cast<float>(engine::wholeNumberAt(table, key, range));
}

/** @returns The value of @p key of @p table in logical units, 0 when the
 * table leaves it out. */
float unitsOrZeroAt(const engine::DataFileTable& table, std::string_view key,
                    engine::WholeNumberRange range) {
  if (!table.table.get().contains(key)) {
    return 0.0F;
  }
  return unitsAt(table, key, range);
}

/** @brief Reads the sheets @p files, adding them to @p texts. */
void addSheetTexts(std::span<const std::filesystem::path> files,
                   std::vector<SheetText>& texts) {
  for (const std::filesystem::path& file : files) {
    texts.push_back(SheetText{.name = logging::genericText(file.stem()),
                              .source = engine::shownPath(file),
                              .text = engine::dataFileContent(file)});
  }
}

/** @throws DuplicateEntityTypeException when two of @p texts share a name:
 * two content folders hold a sheet of that name. */
void refuseSharedNames(std::span<const SheetText> texts) {
  std::set<std::string> names;
  for (const SheetText& text : texts) {
    if (!names.insert(text.name).second) {
      throw DuplicateEntityTypeException(text.name);
    }
  }
}

/** @returns The sheet folder of @p contentFolder. */
std::filesystem::path sheetFolderOf(
    const std::filesystem::path& contentFolder) {
  return contentFolder / ENTITY_SHEET_FOLDER_IN_ASSETS;
}

}  // namespace

std::vector<SheetText> sheetTextsIn(const std::filesystem::path& folder) {
  const std::vector<std::filesystem::path> files = engine::dataFilesIn(folder);
  if (files.empty()) {
    throw NoEntitySheetException(engine::shownPath(folder));
  }
  std::vector<SheetText> texts;
  texts.reserve(files.size());
  addSheetTexts(files, texts);
  checkSheetNames(texts);
  return texts;
}

std::vector<SheetText> sheetTextsInFolders(
    std::span<const std::filesystem::path> contentFolders) {
  if (contentFolders.empty()) {
    throw NoEntitySheetException(std::string{});
  }
  std::vector<SheetText> texts =
      sheetTextsIn(sheetFolderOf(contentFolders.front()));
  for (const std::filesystem::path& folder : contentFolders.subspan(1)) {
    const std::filesystem::path sheets = sheetFolderOf(folder);
    std::error_code error;
    if (std::filesystem::is_directory(sheets, error)) {
      addSheetTexts(engine::dataFilesIn(sheets), texts);
    }
  }
  checkSheetNames(texts);
  refuseSharedNames(texts);
  return texts;
}

void checkSheetNames(std::span<const SheetText> texts) {
  if (texts.size() > identifiers::MAXIMUM_ENTITY_TYPES) {
    throw TooManyEntityTypesException(texts.size(),
                                      identifiers::MAXIMUM_ENTITY_TYPES);
  }
  for (const SheetText& text : texts) {
    if (!identifiers::isContentName(text.name)) {
      throw InvalidEntityTypeNameException(
          text.source, identifiers::MAXIMUM_CONTENT_NAME_LENGTH);
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
  return CollisionBox{
      .width = unitsAt(hitbox, WIDTH_KEY, HITBOX_SIDE_RANGE),
      .height = unitsAt(hitbox, HEIGHT_KEY, HITBOX_SIDE_RANGE),
      .offsetX = unitsOrZeroAt(hitbox, OFFSET_X_KEY, HURT_BOX_OFFSET_RANGE),
      .offsetY = unitsOrZeroAt(hitbox, OFFSET_Y_KEY, HURT_BOX_OFFSET_RANGE)};
}

}  // namespace rtype::game
