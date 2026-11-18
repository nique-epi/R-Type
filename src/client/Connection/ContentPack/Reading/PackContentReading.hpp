#pragma once

#include <span>
#include "ContentFile.hpp"
#include "PackContent.hpp"
#include "PackFolders.hpp"

namespace rtype::client {

/**
 * @brief Reads the content of a pack a server plays with: its sheets, under
 * ENTITY_SHEET_FOLDER_IN_ASSETS, give the appearances and the numbers of the
 * types; every other file, a sprite sheet or a file of a background, is
 * written below the pack folder of @p folders, where the appearances find
 * it. The backgrounds of the pack are then read there, as
 * BackgroundCatalog::ofLastFolder() reads what is laid over the assets of
 * @p folders: a file a background names is one of the pack or, by its asset
 * id, one of those assets.
 * @param files Files a decoded pack holds, so their paths stay below the
 * pack folder.
 * @throws What EntityAppearances::fromTexts() and
 * game::numberedEntityTypesOf() throw for a sheet this client cannot read;
 * ForeignSpriteSheetException when a sheet names a sprite sheet the pack does
 * not hold, so a pack never makes the client read a file of its own;
 * ContentFileWriteException when a file cannot be written; and what
 * BackgroundCatalog::ofLastFolder() throws for a background this client
 * cannot read. Nothing is written before the sheets are checked, and a file
 * already written at its size is not written again.
 */
[[nodiscard]] PackContent packContentOf(
    std::span<const identifiers::ContentFile> files,
    const PackFolders& folders);

}  // namespace rtype::client
