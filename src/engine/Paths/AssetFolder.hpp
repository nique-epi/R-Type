#pragma once

#include <filesystem>

namespace rtype::engine {

/**
 * @brief Finds the assets folder next to the running executable, in the folder
 * above it, or in the Resources folder of its macOS app, whatever folder the
 * program was started from.
 * @throws UnknownExecutablePathException when the system does not tell where
 * the executable is.
 * @throws AssetFolderNotFoundException when none of the three folders is an
 * assets folder.
 */
std::filesystem::path locateAssetFolder();

/** @brief Finds the assets folder next to an executable folder, in the folder
 * above it, or in the Resources folder beside it. */
std::filesystem::path findAssetFolder(
    const std::filesystem::path& executableFolder);

}  // namespace rtype::engine
