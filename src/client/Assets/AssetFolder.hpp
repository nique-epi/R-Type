#pragma once

#include <filesystem>

namespace rtype::client {

/** @brief Finds the assets folder next to the running executable, or in the
 * folder above it. */
std::filesystem::path locateAssetFolder();

/** @brief Finds the assets folder next to an executable folder, or in the
 * folder above it. */
std::filesystem::path findAssetFolder(
    const std::filesystem::path& executableFolder);

}  // namespace rtype::client
