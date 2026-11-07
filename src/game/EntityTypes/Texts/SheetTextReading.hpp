#pragma once

#include <filesystem>
#include <span>
#include <string_view>
#include <vector>
#include "DataFileTable.hpp"
#include "SheetText.hpp"

namespace rtype::game {

/**
 * @brief Reads the text of every sheet of @p folder: each `.toml` file, the
 * name of the file without its extension naming the type, any other file left
 * alone. The server reads the gameplay of these texts, the client their
 * appearance, so both find the same types under the same names.
 * @returns The texts, in the order of their names, each with its absolute path
 * for the errors, checked by checkSheetNames().
 * @throws engine::UnreadableDataFileException when the folder is missing or
 * cannot be listed, or a file cannot be read; NoEntitySheetException when the
 * folder holds no `.toml` file; and what checkSheetNames() throws.
 */
[[nodiscard]] std::vector<SheetText> sheetTextsIn(
    const std::filesystem::path& folder);

/**
 * @brief Reads the sheets of every content folder, the assets first then
 * each resource (see engine::contentFoldersOf()): the
 * ENTITY_SHEET_FOLDER_IN_ASSETS of the first folder, as sheetTextsIn() does,
 * then that of each other folder that has one; a resource may hold no sheet. @p
 * contentFolders is never empty: it starts with the assets folder.
 * @returns The texts, folder after folder, each in the order of its names,
 * checked by checkSheetNames() all together.
 * @throws What sheetTextsIn() throws for the first folder, and
 * engine::UnreadableDataFileException when a sheet of another cannot be read.
 */
[[nodiscard]] std::vector<SheetText> sheetTextsInFolders(
    std::span<const std::filesystem::path> contentFolders);

/**
 * @throws TooManyEntityTypesException when @p texts hold more than
 * MAXIMUM_ENTITY_TYPES sheets, and InvalidEntityTypeNameException, naming the
 * file, for a name other than 1 to MAXIMUM_ENTITY_TYPE_NAME_LENGTH bytes of
 * ENTITY_TYPE_NAME_CHARACTERS.
 */
void checkSheetNames(std::span<const SheetText> texts);

/**
 * @brief Reads the root of a sheet for one of the two programs: the server
 * asks for GAMEPLAY_KEY, the client for APPEARANCE_KEY, and neither reads the
 * table of the other, only checks it is there.
 * @param root The root of the sheet, as engine::rootOf() returns it.
 * @param key One of SHEET_ROOT_KEYS.
 * @returns The table @p key of @p root, once the root holds no other key than
 * SHEET_ROOT_KEYS and the other table of SHEET_ROOT_KEYS is a table.
 * @throws engine::UnknownDataFileKeyException for another key of the root;
 * engine::MissingDataFileKeyException or engine::DataFileTableExpectedException
 * when the other table, then the table @p key, is missing or is not a table.
 */
[[nodiscard]] engine::DataFileTable sheetTableOf(
    const engine::DataFileTable& root, std::string_view key);

}  // namespace rtype::game
