#pragma once

#include <filesystem>
#include <vector>
#include "ContentFile.hpp"

namespace rtype::game {

/**
 * @brief Lists the content of @p assetFolder a client needs to draw what a
 * server plays: every sheet of ENTITY_SHEET_FOLDER_IN_ASSETS, and the sprite
 * sheet each one names under SPRITE_SHEET_KEY, read once however many sheets
 * name it. The server and the client list it the same way, so the same
 * folders give the same files.
 * @returns The files, in the order of their paths.
 * @throws What sheetTextsIn() throws; engine::DataFileSyntaxException,
 * MissingDataFileKeyException, DataFileTableExpectedException or
 * DataFileTextExpectedException when a sheet names no sprite sheet; and
 * engine::UnreadableDataFileException when a sprite sheet cannot be read.
 */
[[nodiscard]] std::vector<identifiers::ContentFile> contentFilesIn(
    const std::filesystem::path& assetFolder);

}  // namespace rtype::game
