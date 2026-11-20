#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <vector>
#include "ContentFile.hpp"
#include "SheetText.hpp"

namespace rtype::game {

/**
 * @brief Lists the content of @p contentFolders a client needs to draw what a
 * server plays: every sheet of their ENTITY_SHEET_FOLDER_IN_ASSETS (see
 * sheetTextsInFolders()) with its mask file when it has one, and the sprite
 * sheet each one names under
 * SPRITE_SHEET_KEY and the sound files its table SOUNDS_TABLE_KEY names, each
 * read once however many sheets name it, from the one folder that holds it. The
 * folders are laid over each other, so a sheet of a resource may name a sprite
 * sheet of the assets. Then the backgrounds the resources bring, with the files
 * they name (see backgroundFilesIn()); a file listed already is not listed
 * again. The server and the client list it the same way, so the same folders
 * give the same files.
 * @param contentFolders The assets folder, then the folder of each resource
 * started (see engine::contentFoldersOf()); a client lists its assets alone.
 * @returns The files, in the order of their paths.
 * @throws What sheetTextsInFolders() throws; engine::DataFileSyntaxException,
 * MissingDataFileKeyException, DataFileTableExpectedException or
 * DataFileTextExpectedException when a sheet names no sprite sheet;
 * SharedContentFileException when two folders hold the sprite sheet a sheet
 * names; engine::UnreadableDataFileException when none does; and what
 * backgroundFilesIn() throws.
 */
[[nodiscard]] std::vector<identifiers::ContentFile> contentFilesIn(
    std::span<const std::filesystem::path> contentFolders);

/** @returns Whether @p path names a sheet: a `.toml` file right in
 * ENTITY_SHEET_FOLDER_IN_ASSETS, as contentFilesIn() lists it. */
[[nodiscard]] bool isSheetPath(const std::string& path);

/** @returns Whether @p path names the mask file of a sheet: a
 * MASK_FILE_EXTENSION file right in the MASK_FOLDER_OF_SHEETS of
 * ENTITY_SHEET_FOLDER_IN_ASSETS, as contentFilesIn() lists it. */
[[nodiscard]] bool isMaskPath(const std::string& path);

/**
 * @returns The sheets among @p files, a `.toml` file right in
 * ENTITY_SHEET_FOLDER_IN_ASSETS each, named after its file, in their order,
 * each with the mask file of @p files at its maskPathOf(); the errors call
 * each file by its path below @p folder.
 */
[[nodiscard]] std::vector<SheetText> sheetTextsOf(
    std::span<const identifiers::ContentFile> files,
    const std::filesystem::path& folder);

}  // namespace rtype::game
