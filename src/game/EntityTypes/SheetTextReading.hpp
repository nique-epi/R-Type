#pragma once

#include <filesystem>
#include <span>
#include <vector>
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
 * @throws TooManyEntityTypesException when @p texts hold more than
 * MAXIMUM_ENTITY_TYPES sheets, and InvalidEntityTypeNameException, naming the
 * file, for a name other than 1 to MAXIMUM_ENTITY_TYPE_NAME_LENGTH bytes of
 * ENTITY_TYPE_NAME_CHARACTERS.
 */
void checkSheetNames(std::span<const SheetText> texts);

}  // namespace rtype::game
