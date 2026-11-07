#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <vector>
#include "ResourcePlaces.hpp"

namespace rtype::engine {

/**
 * @returns Where the program finds its content: the assets folder it ships
 * (see locateAssetFolder()), and the RESOURCE_FOLDER_NAME folder beside it.
 * @throws What locateAssetFolder() throws.
 */
[[nodiscard]] ResourcePlaces locateResourcePlaces();

/**
 * @brief Lays the resources @p names start over the assets of @p places.
 *
 * Each resource is the folder of its name in the resource folder, with a
 * manifest (see readResourceManifest()); the resources it depends on come
 * before it, each once, and a resource another one already brought is not
 * listed again.
 *
 * @returns The assets folder, then the folder of each resource, in the order
 * their content is laid: the order the lines name them, dependencies first.
 * @throws ResourceNotFoundException for a resource without a manifest,
 * ResourceDependencyCycleException when the dependencies make a circle, and
 * what readResourceManifest() throws.
 */
[[nodiscard]] std::vector<std::filesystem::path> contentFoldersOf(
    const ResourcePlaces& places, std::span<const std::string> names);

/**
 * @returns The file @p relativeFile of the last of @p contentFolders that
 * holds it, so a resource replaces a file of the assets or of a resource laid
 * before it; the file in the first folder when none holds it, for the error
 * of its reader to name it. @p contentFolders is never empty.
 */
[[nodiscard]] std::filesystem::path topmostContentFile(
    std::span<const std::filesystem::path> contentFolders,
    const std::filesystem::path& relativeFile);

}  // namespace rtype::engine
