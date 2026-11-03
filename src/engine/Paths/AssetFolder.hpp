#pragma once

#include <filesystem>

namespace rtype::engine {

/**
 * @brief Finds the assets folder next to the running executable, or in the
 * folder above it, whatever folder the program was started from.
 * @throws UnknownExecutablePathException when the system does not tell where
 * the executable is.
 * @throws AssetFolderNotFoundException when neither folder is an assets
 * folder.
 */
std::filesystem::path locateAssetFolder();

/** @brief Finds the assets folder next to an executable folder, or in the
 * folder above it. */
std::filesystem::path findAssetFolder(
    const std::filesystem::path& executableFolder);

}  // namespace rtype::engine
