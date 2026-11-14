#pragma once

#include <filesystem>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
#include "CollisionBox.hpp"
#include "DataFileTable.hpp"
#include "MaskSheet.hpp"
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
 * then that of each other folder that has one; a resource may hold no sheet.
 * @p contentFolders starts with the assets folder; none throws
 * NoEntitySheetException.
 * @returns The texts, folder after folder, each in the order of its names,
 * checked by checkSheetNames() all together.
 * @throws What sheetTextsIn() throws for the first folder,
 * engine::UnreadableDataFileException when a sheet of another cannot be read,
 * and DuplicateEntityTypeException when two folders hold a sheet of the same
 * name: a resource adds sheets, it never replaces one.
 */
[[nodiscard]] std::vector<SheetText> sheetTextsInFolders(
    std::span<const std::filesystem::path> contentFolders);

/**
 * @throws TooManyEntityTypesException when @p texts hold more than
 * MAXIMUM_ENTITY_TYPES sheets, and InvalidEntityTypeNameException, naming the
 * file, for a name other than 1 to MAXIMUM_CONTENT_NAME_LENGTH bytes of
 * CONTENT_NAME_CHARACTERS.
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

/**
 * @returns The boxes `[[gameplay.hitbox.box]]` of @p gameplay lists, in the
 * order of the file and in logical units; none for a hitbox that gives the
 * size of its one box. The server collides its entities with them, the client
 * tests its own missiles against them, so both read them the same way.
 * @throws engine::MissingDataFileKeyException or
 * DataFileTableExpectedException when `[gameplay.hitbox]` is missing or is not
 * a table; UnknownDataFileKeyException for a key of it other than
 * HITBOX_TABLE_KEYS, or a key of a box other than HITBOX_KEYS;
 * InconsistentEntitySheetException when it gives a size or an offset beside
 * its boxes, or lists none; and what engine::tablesAt() and
 * engine::wholeNumberAt() throw for the array and for a side outside
 * HITBOX_SIDE_RANGE.
 */
[[nodiscard]] std::vector<CollisionBox> hitboxPartsOf(
    const engine::DataFileTable& gameplay);

/**
 * @returns The box `[gameplay.hitbox]` of @p gameplay describes, in logical
 * units: its one box, or the rectangle that encloses the boxes it lists.
 * @throws What hitboxPartsOf() throws; UnknownDataFileKeyException for a key
 * other than HITBOX_KEYS in a hitbox of one box.
 */
[[nodiscard]] CollisionBox hitboxOf(const engine::DataFileTable& gameplay);

/**
 * @returns The mask `[gameplay.mask]` of @p gameplay draws, row after row,
 * and its erosion, 0 when left out; nothing for a sheet without mask. Both
 * programs read a mask this way, so they agree on it; no system collides
 * with it yet.
 * @throws UnknownDataFileKeyException for a key other than MASK_KEYS; what
 * engine::textsAt() throws for `rows`; InconsistentEntitySheetException, at
 * the row, for a row of another width than the first or with a character
 * other than DRAWN_MASK_PIXEL and CLEAR_MASK_PIXEL, and at `rows` for no row,
 * a side above engine::MAXIMUM_MASK_SIDE or no drawn pixel; and
 * DataFileValueOutOfRangeException for an erosion outside MASK_EROSION_RANGE.
 */
[[nodiscard]] std::optional<MaskSheet> maskOf(
    const engine::DataFileTable& gameplay);

}  // namespace rtype::game
