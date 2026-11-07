#include "PackContentReading.hpp"
#include <filesystem>
#include <fstream>
#include <ios>
#include <set>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "ClientException.hpp"
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "EntityAppearances.hpp"
#include "NumberedEntityTypes.hpp"
#include "PackContent.hpp"
#include "PathText.hpp"
#include "SheetText.hpp"

namespace rtype::client {

namespace {

/** @brief Writes @p file below @p folder, with the folders on its way.
 * @throws ContentFileWriteException when the disk refuses it. */
void write(const identifiers::ContentFile& file,
           const std::filesystem::path& folder) {
  const std::filesystem::path written = folder / file.path;
  std::error_code error;
  if (std::filesystem::file_size(written, error) == file.bytes.size() &&
      !error) {
    return;
  }
  std::filesystem::create_directories(written.parent_path(), error);
  std::ofstream stream{written, std::ios::binary | std::ios::trunc};
  stream.write(file.bytes.data(),
               static_cast<std::streamsize>(file.bytes.size()));
  if (!stream) {
    throw ContentFileWriteException(logging::genericText(written));
  }
}

/** @returns The whole path, below @p spriteFolder, of each file of @p files
 * that is no sheet: the sprite sheets an appearance of the pack may name. */
std::set<std::string> spriteSheetsOf(
    std::span<const identifiers::ContentFile> files,
    const std::filesystem::path& spriteFolder) {
  std::set<std::string> spriteSheets;
  for (const identifiers::ContentFile& file : files) {
    if (!game::isSheetPath(file.path)) {
      spriteSheets.insert(logging::genericText(spriteFolder / file.path));
    }
  }
  return spriteSheets;
}

/** @throws ForeignSpriteSheetException when an appearance of @p appearances
 * names a sprite sheet that is not among @p spriteSheets. */
void requireOwnSpriteSheets(const EntityAppearances& appearances,
                            const std::set<std::string>& spriteSheets) {
  for (const std::string& sheet : appearances.sheets()) {
    if (!spriteSheets.contains(sheet)) {
      throw ForeignSpriteSheetException(sheet);
    }
  }
}

}  // namespace

PackContent packContentOf(std::span<const identifiers::ContentFile> files,
                          const std::filesystem::path& spriteFolder) {
  const std::vector<game::SheetText> sheets =
      game::sheetTextsOf(files, spriteFolder);
  EntityAppearances appearances =
      EntityAppearances::fromTexts(sheets, spriteFolder);
  requireOwnSpriteSheets(appearances, spriteSheetsOf(files, spriteFolder));
  for (const identifiers::ContentFile& file : files) {
    if (!game::isSheetPath(file.path)) {
      write(file, spriteFolder);
    }
  }
  return PackContent{.appearances = std::move(appearances),
                     .types = game::numberedEntityTypesOf(sheets)};
}

}  // namespace rtype::client
