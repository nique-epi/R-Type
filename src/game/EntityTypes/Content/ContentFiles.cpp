#include "ContentFiles.hpp"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <iterator>
#include <set>
#include <span>
#include <string>
#include <system_error>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>
#include "ContentFile.hpp"
#include "DataFileConstants.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "GameException.hpp"
#include "PathText.hpp"
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

/** @returns The folders of @p contentFolders that hold @p path. */
std::vector<std::filesystem::path> foldersHolding(
    std::span<const std::filesystem::path> contentFolders,
    const std::string& path) {
  std::vector<std::filesystem::path> holders;
  for (const std::filesystem::path& folder : contentFolders) {
    std::error_code error;
    if (std::filesystem::is_regular_file(folder / path, error)) {
      holders.push_back(folder);
    }
  }
  return holders;
}

/**
 * @returns The bytes of the file at @p path in the one folder of
 * @p contentFolders that holds it.
 * @throws SharedContentFileException when several hold it, and
 * engine::UnreadableDataFileException, naming it in the first folder, when
 * none does.
 */
std::string contentOf(std::span<const std::filesystem::path> contentFolders,
                      const std::string& path) {
  const std::vector<std::filesystem::path> holders =
      foldersHolding(contentFolders, path);
  if (holders.size() > 1) {
    std::vector<std::string> shown;
    std::ranges::transform(holders, std::back_inserter(shown),
                           [](const std::filesystem::path& folder) {
                             return engine::shownPath(folder);
                           });
    throw SharedContentFileException(path, shown);
  }
  const std::filesystem::path folder =
      holders.empty() ? contentFolders.front() : holders.front();
  return engine::dataFileContent(folder / path);
}

}  // namespace

bool isSheetPath(const std::string& path) {
  const std::filesystem::path file{path};
  return file.parent_path() == ENTITY_SHEET_FOLDER_IN_ASSETS &&
         file.extension() == engine::DATA_FILE_EXTENSION;
}

std::vector<SheetText> sheetTextsOf(
    std::span<const identifiers::ContentFile> files,
    const std::filesystem::path& folder) {
  std::vector<SheetText> sheets;
  for (const identifiers::ContentFile& file : files) {
    if (isSheetPath(file.path)) {
      const std::filesystem::path path{file.path};
      sheets.push_back(SheetText{.name = logging::genericText(path.stem()),
                                 .source = engine::shownPath(folder / path),
                                 .text = file.bytes});
    }
  }
  return sheets;
}

std::vector<identifiers::ContentFile> contentFilesIn(
    std::span<const std::filesystem::path> contentFolders) {
  std::vector<SheetText> sheets = sheetTextsInFolders(contentFolders);
  std::vector<identifiers::ContentFile> files;
  std::set<std::string> spriteSheets;
  for (SheetText& sheet : sheets) {
    spriteSheets.insert(spriteSheetOf(sheet));
    files.push_back(identifiers::ContentFile{.path = sheetPathOf(sheet.name),
                                             .bytes = std::move(sheet.text)});
  }
  for (const std::string& spriteSheet : spriteSheets) {
    files.push_back(identifiers::ContentFile{
        .path = spriteSheet, .bytes = contentOf(contentFolders, spriteSheet)});
  }
  std::ranges::sort(files, std::less{}, &identifiers::ContentFile::path);
  return files;
}

}  // namespace rtype::game
