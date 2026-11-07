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
 * sheetTextsInFolders()), and the sprite sheet each one names under
 * SPRITE_SHEET_KEY, read once however many sheets name it, from the one
 * folder that holds it. The folders are laid over each other, so a sheet of a
 * resource may name a sprite sheet of the assets. The server and the client
 * list it the same way, so the same folders give the same files.
 * @param contentFolders The assets folder, then the folder of each resource
 * started (see engine::contentFoldersOf()); a client lists its assets alone.
 * @returns The files, in the order of their paths.
 * @throws What sheetTextsInFolders() throws; DuplicateEntityTypeException when
 * two folders hold a sheet of the same name; engine::DataFileSyntaxException,
 * MissingDataFileKeyException, DataFileTableExpectedException or
 * DataFileTextExpectedException when a sheet names no sprite sheet;
 * SharedContentFileException when two folders hold the sprite sheet a sheet
 * names; and engine::UnreadableDataFileException when none does.
 */
[[nodiscard]] std::vector<identifiers::ContentFile> contentFilesIn(
    std::span<const std::filesystem::path> contentFolders);

/** @returns Whether @p path names a sheet: a `.toml` file right in
 * ENTITY_SHEET_FOLDER_IN_ASSETS, as contentFilesIn() lists it. */
[[nodiscard]] bool isSheetPath(const std::string& path);

/**
 * @returns The sheets among @p files, a `.toml` file right in
 * ENTITY_SHEET_FOLDER_IN_ASSETS each, named after its file, in their order;
 * the errors call each the file of its path below @p folder.
 */
[[nodiscard]] std::vector<SheetText> sheetTextsOf(
    std::span<const identifiers::ContentFile> files,
    const std::filesystem::path& folder);

}  // namespace rtype::game
