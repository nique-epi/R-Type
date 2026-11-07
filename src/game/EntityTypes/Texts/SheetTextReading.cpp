#include "SheetTextReading.hpp"
#include <filesystem>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeName.hpp"
#include "GameException.hpp"
#include "PathText.hpp"
#include "SheetText.hpp"

namespace rtype::game {

namespace {

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

}  // namespace rtype::game
