#pragma once

#include <filesystem>
#include <span>
#include <vector>
#include "ScriptSource.hpp"

namespace rtype::game {

/**
 * @returns The server scripts of @p contentFolders: every SCRIPT_FILE_EXTENSION
 * file right in the SCRIPT_FOLDER_IN_CONTENT of each folder, folder after
 * folder, each in the order of its names, named after the folder and the file
 * (`crimson-drifter/server/charge.luau`). A folder without scripts gives none.
 * @throws engine::UnreadableDataFileException when a script cannot be read.
 */
[[nodiscard]] std::vector<engine::ScriptSource> scriptSourcesIn(
    std::span<const std::filesystem::path> contentFolders);

}  // namespace rtype::game
