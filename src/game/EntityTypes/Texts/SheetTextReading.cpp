#include "SheetTextReading.hpp"
#include <algorithm>
#include <filesystem>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeName.hpp"
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

/** @returns The box @p table describes: its size, and its offset, 0 without
 * it. @throws engine::UnknownDataFileKeyException for a key other than
 * HITBOX_KEYS. */
CollisionBox boxAt(const engine::DataFileTable& table) {
  engine::refuseUnknownKeys(table, HITBOX_KEYS);
  return CollisionBox{
      .width = unitsAt(table, WIDTH_KEY, HITBOX_SIDE_RANGE),
      .height = unitsAt(table, HEIGHT_KEY, HITBOX_SIDE_RANGE),
      .offsetX = unitsOrZeroAt(table, OFFSET_X_KEY, HURT_BOX_OFFSET_RANGE),
      .offsetY = unitsOrZeroAt(table, OFFSET_Y_KEY, HURT_BOX_OFFSET_RANGE)};
}

/** @throws InconsistentEntitySheetException when @p hitbox, which lists its
 * boxes, gives a size or an offset of its own too. */
void refuseASizeBesideBoxes(const engine::DataFileTable& hitbox) {
  for (const std::string_view key : HITBOX_KEYS) {
    if (hitbox.table.get().contains(key)) {
      throw InconsistentEntitySheetException(engine::placeOfKey(hitbox, key),
                                             HITBOX_SHAPE_RULE);
    }
  }
}

/** @returns The smallest box that holds every box of @p parts, which is not
 * empty. */
CollisionBox enclosingBoxOf(std::span<const CollisionBox> parts) {
  float left = parts.front().offsetX - (parts.front().width * engine::HALF);
  float right = parts.front().offsetX + (parts.front().width * engine::HALF);
  float top = parts.front().offsetY - (parts.front().height * engine::HALF);
  float bottom = parts.front().offsetY + (parts.front().height * engine::HALF);
  for (const CollisionBox& part : parts) {
    left = std::min(left, part.offsetX - (part.width * engine::HALF));
    right = std::max(right, part.offsetX + (part.width * engine::HALF));
    top = std::min(top, part.offsetY - (part.height * engine::HALF));
    bottom = std::max(bottom, part.offsetY + (part.height * engine::HALF));
  }
  return CollisionBox{.width = right - left,
                      .height = bottom - top,
                      .offsetX = (left + right) * engine::HALF,
                      .offsetY = (top + bottom) * engine::HALF};
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

std::vector<CollisionBox> hitboxPartsOf(const engine::DataFileTable& gameplay) {
  const engine::DataFileTable hitbox = engine::tableAt(gameplay, HITBOX_KEY);
  if (!hitbox.table.get().contains(BOX_KEY)) {
    return {};
  }
  engine::refuseUnknownKeys(hitbox, HITBOX_TABLE_KEYS);
  refuseASizeBesideBoxes(hitbox);
  std::vector<CollisionBox> parts;
  for (const engine::DataFileTable& box : engine::tablesAt(hitbox, BOX_KEY)) {
    parts.push_back(boxAt(box));
  }
  if (parts.empty()) {
    throw InconsistentEntitySheetException(engine::placeOfKey(hitbox, BOX_KEY),
                                           HITBOX_BOXES_RULE);
  }
  return parts;
}

CollisionBox hitboxOf(const engine::DataFileTable& gameplay) {
  const std::vector<CollisionBox> parts = hitboxPartsOf(gameplay);
  if (!parts.empty()) {
    return enclosingBoxOf(parts);
  }
  return boxAt(engine::tableAt(gameplay, HITBOX_KEY));
}

}  // namespace rtype::game
