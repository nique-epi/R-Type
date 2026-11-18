#pragma once

#include <filesystem>
#include <span>
#include <vector>
#include "ContentFile.hpp"

namespace rtype::game {

/**
 * @brief Lists the backgrounds the resources of @p contentFolders bring, as
 * a client needs them to show them: every background file right in the
 * BACKGROUND_FOLDER_IN_ASSETS folder of a resource, and every file one of
 * them names that a resource holds.
 *
 * A later resource replaces the background of the same name an earlier one
 * holds, and a file a background names is read from the last resource that
 * holds it. The backgrounds of the first folder, the assets, are never
 * listed, nor a file only the assets hold: every client has its own.
 *
 * @param contentFolders The assets folder, then the folder of each resource
 * started (see engine::contentFoldersOf()); never empty.
 * @returns The files, in the order of their paths; none for the assets
 * alone.
 * @throws engine::UnreadableDataFileException when a folder of backgrounds
 * cannot be listed, a file cannot be read, or a background names a file no
 * folder holds, named in the assets; engine::DataFileSyntaxException,
 * MissingDataFileKeyException, DataFileTableExpectedException,
 * DataFileTablesExpectedException or DataFileTextExpectedException when a
 * background file does not name its files as a background does.
 */
[[nodiscard]] std::vector<identifiers::ContentFile> backgroundFilesIn(
    std::span<const std::filesystem::path> contentFolders);

}  // namespace rtype::game
