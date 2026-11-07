#pragma once

#include <filesystem>
#include <span>
#include "ContentFile.hpp"
#include "PackContent.hpp"

namespace rtype::client {

/**
 * @brief Reads the content of a pack a server plays with: its sheets, under
 * ENTITY_SHEET_FOLDER_IN_ASSETS, give the appearances and the numbers of the
 * types; every other file, a sprite sheet, is written below @p spriteFolder,
 * where the appearances find it.
 * @param files Files a decoded pack holds, so their paths stay below
 * @p spriteFolder.
 * @throws What EntityAppearances::fromTexts() and
 * game::numberedEntityTypesOf() throw for a sheet this client cannot read,
 * and ContentFileWriteException when a sprite sheet cannot be written.
 */
[[nodiscard]] PackContent packContentOf(
    std::span<const identifiers::ContentFile> files,
    const std::filesystem::path& spriteFolder);

}  // namespace rtype::client
