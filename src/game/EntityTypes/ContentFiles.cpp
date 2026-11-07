#include "ContentFiles.hpp"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <set>
#include <string>
#include <toml++/toml.hpp>
#include <vector>
#include "ContentFile.hpp"
#include "DataFileConstants.hpp"
#include "DataFileReading.hpp"
#include "EntitySheetConstants.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"

namespace rtype::game {

namespace {

/** @returns The path of the sheet of the type @p name in the assets folder. */
std::string sheetPathOf(const std::string& name) {
  return std::string{ENTITY_SHEET_FOLDER_IN_ASSETS} + ASSET_PATH_SEPARATOR +
         name + std::string{engine::DATA_FILE_EXTENSION};
}

/** @returns The sprite sheet the `[appearance]` of @p sheet names. */
std::string spriteSheetOf(const SheetText& sheet) {
  const toml::table root = engine::parseDataFile(sheet.text, sheet.source);
  const engine::DataFileTable appearance =
      engine::tableAt(engine::rootOf(root, sheet.source), APPEARANCE_KEY);
  return engine::textAt(appearance, SPRITE_SHEET_KEY);
}

}  // namespace

std::vector<identifiers::ContentFile> contentFilesIn(
    const std::filesystem::path& assetFolder) {
  const std::vector<SheetText> sheets =
      sheetTextsIn(assetFolder / ENTITY_SHEET_FOLDER_IN_ASSETS);
  std::vector<identifiers::ContentFile> files;
  std::set<std::string> spriteSheets;
  for (const SheetText& sheet : sheets) {
    files.push_back(identifiers::ContentFile{.path = sheetPathOf(sheet.name),
                                             .bytes = sheet.text});
    spriteSheets.insert(spriteSheetOf(sheet));
  }
  for (const std::string& spriteSheet : spriteSheets) {
    files.push_back(identifiers::ContentFile{
        .path = spriteSheet,
        .bytes = engine::dataFileContent(assetFolder / spriteSheet)});
  }
  std::ranges::sort(files, std::less{}, &identifiers::ContentFile::path);
  return files;
}

}  // namespace rtype::game
