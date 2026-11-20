#include "PackContentReading.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <ios>
#include <memory>
#include <set>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "BackgroundCatalog.hpp"
#include "BackgroundFiles.hpp"
#include "ClientException.hpp"
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "EntityAppearances.hpp"
#include "NumberedEntityTypes.hpp"
#include "PackContent.hpp"
#include "PackFolders.hpp"
#include "PathText.hpp"
#include "SheetText.hpp"
#include "ShipGunReading.hpp"

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

/** @returns Whether @p path names a file a pack writes below its folder, a
 * sprite sheet or a file of a background: any file but a sheet and the mask
 * file of a sheet. */
bool isSpriteSheetPath(const std::string& path) {
  return !game::isSheetPath(path) && !game::isMaskPath(path);
}

/** @returns The whole path, below @p spriteFolder, of each file of @p files
 * that is a sprite sheet: those an appearance of the pack may name. */
std::set<std::string> spriteSheetsOf(
    std::span<const identifiers::ContentFile> files,
    const std::filesystem::path& spriteFolder) {
  std::set<std::string> spriteSheets;
  for (const identifiers::ContentFile& file : files) {
    if (isSpriteSheetPath(file.path)) {
      spriteSheets.insert(logging::genericText(spriteFolder / file.path));
    }
  }
  return spriteSheets;
}

/** @throws ForeignSpriteSheetException when an appearance of @p appearances
 * names a sprite sheet or a sound that is not among @p spriteSheets, the
 * files the pack writes. */
void requireOwnSpriteSheets(const EntityAppearances& appearances,
                            const std::set<std::string>& spriteSheets) {
  for (const std::vector<std::string>& named :
       {appearances.sheets(), appearances.sounds()}) {
    for (const std::string& file : named) {
      if (!spriteSheets.contains(file)) {
        throw ForeignSpriteSheetException(file);
      }
    }
  }
}

/** @returns The backgrounds of the pack written below the pack folder of
 * @p folders, laid over its assets; nullptr when @p files hold none. */
std::shared_ptr<const BackgroundCatalog> backgroundsOf(
    std::span<const identifiers::ContentFile> files,
    const PackFolders& folders) {
  if (std::ranges::none_of(files, game::isBackgroundPath,
                           &identifiers::ContentFile::path)) {
    return nullptr;
  }
  const std::array contentFolders{folders.assets, folders.pack};
  return std::make_shared<const BackgroundCatalog>(
      BackgroundCatalog::ofLastFolder(contentFolders));
}

}  // namespace

PackContent packContentOf(std::span<const identifiers::ContentFile> files,
                          const PackFolders& folders) {
  const std::vector<game::SheetText> sheets =
      game::sheetTextsOf(files, folders.pack);
  EntityAppearances appearances =
      EntityAppearances::fromTexts(sheets, folders.pack);
  requireOwnSpriteSheets(appearances, spriteSheetsOf(files, folders.pack));
  for (const identifiers::ContentFile& file : files) {
    if (isSpriteSheetPath(file.path)) {
      write(file, folders.pack);
    }
  }
  return PackContent{.appearances = std::move(appearances),
                     .types = game::numberedEntityTypesOf(sheets),
                     .gun = game::shipGunOrNoneOf(sheets),
                     .backgrounds = backgroundsOf(files, folders),
                     .folder = folders.pack};
}

}  // namespace rtype::client
